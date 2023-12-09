#include "paintarea.h"
#include <QGraphicsSceneMouseEvent>
#include "opencv2/core.hpp"
#include "opencv2/opencv.hpp"
#include <QPainter>
#include "../Helper/imagehelper.h"
#include "../Paint/Widgets/history.h"
#include <QClipboard>
#include <QApplication>
#include <QDebug>

PaintArea::PaintArea(QWidget* parent) :
    QGraphicsScene(parent),
    container(NULL),
    inLayer(false),
    press(false),
    mouseGrabber(NULL),
    rootLayer(new RootLayer(this)),
    clipLayer(NULL)
{
}

void PaintArea::mousePressEvent(QGraphicsSceneMouseEvent *event){
    press = true;
    for(auto& layer: layers){
        if((layer->isEnable() || layer->isErase()) && layer->contains(layer->mapFromScene(event->scenePos()))){
//            sendEvent(layer, event);
            inLayer = true;
            mouseGrabber = layer;
        }
    }
    if(!inLayer){
        if(container != NULL) container->layerMousePressEvent(event);
    }
    QGraphicsScene::mousePressEvent(event);
}

void PaintArea::mouseMoveEvent(QGraphicsSceneMouseEvent *event){
    QGraphicsScene::mouseMoveEvent(event);
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
        return;
    }
    else{
        press = false;
    }
//    if(press) sendEvent(mouseGrabber, event);
    inLayer = false;
    QGraphicsScene::mouseReleaseEvent(event);
}

bool PaintArea::save(SaveType type, const QString &path){
    if(path == "") {
        return false;
    }
    for(auto& layer: layers){
        layer->prepareSave();
    }
    QRectF bound;
    for(auto& layer: layers){
        bound = bound.united(layer->boundingRect());
    }

    if(bound == QRectF(0, 0, 0, 0))
        return false;

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
    if(type == ClipBoard){
        QClipboard *clip=QApplication::clipboard();
        clip->setImage(ImageHelper::Mat2QImage(ans));
    }
    else{
        cv::imwrite(path.toLocal8Bit().toStdString(), ans);
        History_data::save_type saveType = History_data::Editable;
        if(type == Persist) saveType = History_data::Persist;
        History::instance()->log(saveType, path);
    }
    for(auto& layer: layers){
        layer->endSave();
    }
    return true;
}

void PaintArea::setEraseEnable(bool enable){
    for(auto iter=layers.begin(); iter!=layers.end(); iter++){
        iter.value()->setErase(enable);
    }
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
    if(clipLayer != NULL) {
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

void PaintArea::reset(){
    this->image = QImage();
    this->modifiedImage = QImage();
    this->modified = false;
    imageValid = false;
    this->container = NULL;
    inLayer = false;
    focusLayer = NULL;
    for(LayerBase* layer : layers){
        layer->reset();
    }
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
