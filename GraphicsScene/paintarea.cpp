#include "paintarea.h"
#include <QGraphicsSceneMouseEvent>
#include "opencv2/core.hpp"
#include "opencv2/opencv.hpp"
#include <QPainter>
#include "../Helper/imagehelper.h"
#include "../Manager/history.h"
#include <QClipboard>
#include <QApplication>
#include <QDebug>

PaintArea::PaintArea(QWidget* parent) :
    QGraphicsScene(parent),
    container(NULL),
    inLayer(false),
    press(false),
    _save(false),
    mouseGrabber(NULL),
    rootLayer(new RootLayer(this)),
    clipLayer(NULL),
    recorder(new Recorder(this)),
    installed(false)
{
    connect(recorder, &Recorder::recordChange, this, [=](){
        emit recordChange();
    });
}

void PaintArea::mousePressEvent(QGraphicsSceneMouseEvent *event){
    if(installed){
        if(f(event->button())) return;
    }
    for(auto pressFunc : pressFuncs){
        pressFunc(event->scenePos());
    }

    press = true;
    if(event->button() == Qt::BackButton) {
        recorder->back();
    } else if(event->button() == Qt::ForwardButton) {
        recorder->forward();
    }

    for(auto& layer: layers){
        if((layer->isEnable()) && layer->contains(layer->mapFromScene(event->scenePos()))){
//            sendEvent(layer, event);
            inLayer = true;
            mouseGrabber = layer;
        }
    }
    if(!inLayer){
        if(container != NULL) container->layerMousePressEvent(event);
    }
    sendEvent(rootLayer, event);
    QGraphicsScene::mousePressEvent(event);
}

void PaintArea::mouseMoveEvent(QGraphicsSceneMouseEvent *event){
    for(auto moveFunc : moveFuncs){
        moveFunc(event->scenePos());
    }
    if(press){
        sendEvent(rootLayer, event);
        QGraphicsScene::mouseMoveEvent(event);
    }
    if(!inLayer && press){
        if(container != NULL) container->layerMouseMoveEvent(event);
        return;
    }
//    if(press) sendEvent(mouseGrabber, event);
}

void PaintArea::mouseReleaseEvent(QGraphicsSceneMouseEvent *event){
    if(!inLayer && press){
        press = false;
        if(container != NULL) container->layerMouseReleaseEvent(event);
    }
    else{
        press = false;
    }
//    if(press) sendEvent(mouseGrabber, event);
    inLayer = false;
    sendEvent(rootLayer, event);
    QGraphicsScene::mouseReleaseEvent(event);
}

bool PaintArea::save(SaveType type, const QString &path){
    if(path == "" && type != ClipBoard) {
        return false;
    }
    for(auto& layer: layers){
        layer->prepareSave();
    }
    update();
    QRectF bound;
    for(auto& layer: layers){
        bound = bound.united(layer->getSaveRect());
    }

    if(bound == QRectF(0, 0, 0, 0))
        return false;

    cv::Mat ans(bound.height(), bound.width(), CV_8UC4);
    for(int i=0; i<bound.height(); i+=32700) {
        int height = bound.height() - i > 32700 ? 32700 : bound.height() - i;
        QRect temp_rect(bound.left(), bound.top()+i, bound.width(), height);
        QImage image(bound.width(), height, QImage::Format_ARGB32);
        image.fill(Qt::transparent);
        QPainter painter(&image);
        render(&painter, QRectF(QPointF(0, 0), image.size()), temp_rect);
        cv::Mat temp_mat = ImageHelper::QImage2Mat(image);
        temp_mat.copyTo(ans(cv::Rect(0, i, bound.width(), height)));
    }
    if(type == ClipBoard){
        QClipboard *clip=QApplication::clipboard();
        clip->setImage(ImageHelper::Mat2QImage(ans));
    }
    else{
        cv::imwrite(path.toLocal8Bit().toStdString(), ans);
        History::instance()->log(type, path);
    }
    for(auto& layer: layers){
        layer->endSave();
    }
    _save = true;
    return true;
}

QImage PaintArea::getSaveImage(){
    for(auto& layer: layers){
        layer->prepareSave();
    }
    update();
    QRectF bound;
    for(auto& layer: layers){
        bound = bound.united(layer->getSaveRect());
    }

    if(bound == QRectF(0, 0, 0, 0))
        return QImage();

    cv::Mat ans(bound.height(), bound.width(), CV_8UC4);
    for(int i=0; i<bound.height(); i+=32700) {
        int height = bound.height() - i > 32700 ? 32700 : bound.height() - i;
        QRect temp_rect(bound.left(), bound.top()+i, bound.width(), height);
        QImage image(bound.width(), height, QImage::Format_ARGB32);
        //        image.fill(Qt::transparent);
        QPainter painter(&image);
        render(&painter, QRectF(QPointF(0, 0), image.size()), temp_rect);
        cv::Mat temp_mat = ImageHelper::QImage2Mat(image);
        temp_mat.copyTo(ans(cv::Rect(0, i, bound.width(), height)));
    }
    for(auto& layer: layers){
        layer->endSave();
    }
    return ImageHelper::Mat2QImage(ans);
}

QRectF PaintArea::getSaveRect(){
    QRectF bound;
    for(auto& layer: layers){
        bound = bound.united(layer->getSaveRect());
    }
    return bound;
}

void PaintArea::setEraseEnable(bool enable){
    changeFocusLayer(NULL);
    for(auto layer : layers){
        layer->startErase();
    }
    rootLayer->setErase(enable);
}

void PaintArea::setEnable(bool enable, int index){
    for(auto iter=layers.begin(); iter!=layers.end(); iter++){
        iter.value()->setEnable(enable, index);
    }
    if(!enable){
        changeFocusLayer(NULL);
    }
}

void PaintArea::addLayer(LayerBase *layer){
    layer->setParentItem(rootLayer);
    layer->setZValue(layer->getZValue());
    ClipLayerBase* clipLayer = dynamic_cast<ClipLayerBase*>(layer);
    if(clipLayer != NULL && this->clipLayer == NULL) {
        this->clipLayer = clipLayer;
    }
//    addItem(layer);
    LayerManager::addLayer(layer);
}

LayerBase* PaintArea::removeLayer(const QString &name){
    LayerBase* layer = LayerManager::removeLayer(name);
    if(layer != NULL){
        if(layer == this->clipLayer) {
            this->clipLayer = NULL;
        }
        removeItem(layer);
        layer->deleteLater();
    }
    return layer;
}

bool PaintArea::hasFocus(LayerBase *layer){
    return LayerManager::hasFocus(layer);
}

void PaintArea::requestFocus(LayerBase *layer){
    LayerManager::changeFocusLayer(layer);
}

void PaintArea::setContainer(LayerContainer *container){
    this->container = container;
}

void PaintArea::setImage(const QImage& image){
    setSceneRect(0, 0, image.width(), image.height());
    this->image = image;
    imageValid = true;
    for(LayerBase* layer : layers){
        layer->onImageSet();
    }
    update();
}

void PaintArea::reset(bool newImage){
    if(newImage){
        this->image = QImage();
        imageValid = false;
    }
    this->modifiedImage = QImage();
    this->modified = false;
    inLayer = false;
    _save = false;
    focusLayer = NULL;
    auto layerBak = layers;
    for(LayerBase* layer : layerBak){
        layer->reset();
    }
    recorder->reset();
}

QGraphicsItem* PaintArea::getRootLayer(){
    return rootLayer;
}

void PaintArea::removeThis(LayerBase *layer){
    removeLayer(layer->getName());
}

QRectF PaintArea::getClipRect() {
    if(clipLayer == NULL) {
        qWarning() << "未设置clip layer";
        return QRectF();
    }
    return clipLayer->getClipRect();
}

ClipLayerBase* PaintArea::getClipLayer(){
    return clipLayer;
}

void PaintArea::record(RecordBase *record){
    recorder->record(record);
}

Recorder* PaintArea::getRecorder(){
    return recorder;
}

bool PaintArea::isSave(){
    return _save;
}

void PaintArea::installMouseFilter(const std::function<bool (Qt::MouseButton)> &f){
    installed = true;
    this->f = f;
}

void PaintArea::registerMousePressHook(const std::function<void (QPointF)> &f){
    pressFuncs.append(f);
}

void PaintArea::registerMouseMoveHook(const std::function<void (QPointF)> &f){
    moveFuncs.append(f);
}
