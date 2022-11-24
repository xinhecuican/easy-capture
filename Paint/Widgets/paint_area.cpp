#include "Paint_area.h"
#include "recorder.h"
#include "style_manager.h"
#include<QDir>
#include<QDateTime>
#include "history.h"
#include<QTimer>
#include "Helper/debug.h"
#include<QFileDialog>
#include<QMouseEvent>
#include "opencv2/core.hpp"
#include "opencv2/opencv.hpp"
#include "Helper/image_helper.h"
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include<QImageWriter>
#include "Helper/image_helper.h"
#include<QPixmapCache>
#include "Paint/Widgets/Layers/text_layer.h"
#include <QTextCodec>//转码
#include<QScrollBar>
#include "Paint/Widgets/Layers/rect_layer.h"
#include "Manager/config.h"
#include "Manager/window_manager.h"
#include<QLabel>
#include<QGraphicsSceneMouseEvent>
#include "Panels/paint_setting_panel.h"
#include<QApplication>
#include<QClipboard>
#include <QScreen>

Paint_area::Paint_area(QObject* parent, bool enable_clip) : QGraphicsScene(parent)
{
    is_save = false;
    is_clip = enable_clip;
    state = PAINT;
    shape_type = RECTANGLE;
    if(!is_clip)
    {
        pic_layer = new Picture_layer();
        addItem(pic_layer);
        initSettingPanel();
        clip_layer = NULL;
    }
    else
    {
        pic_layer = NULL;
        clip_layer = new ClipLayer();
        connect(clip_layer, &ClipLayer::requestImage, this, [=](){
            prepareSave();
            QRectF bound = clip_layer->getClipRect();
            bound = bound.united(paint_layer->childrenBoundingRect());
            bound = bound.united(shape_layer->childrenBoundingRect());

            if(bound == QRectF(0, 0, 0, 0))
                return;

            QImage image(bound.width(), bound.height(), QImage::Format_ARGB32);
            image.fill(Qt::transparent);
            QPainter painter(&image);
            render(&painter, QRectF(QPointF(0, 0), bound.size()), bound);
            Window_manager::change_window("Paint_window");
            bound.moveTo(0, 0);
            Window_manager::get_window("Paint_window")->set_pic(QPixmap::fromImage(image), bound.toRect());
            endSave();
        });
        addItem(clip_layer);
    }
    paint_layer = new Paint_layer();
    paint_layer->setZValue(1);
    addItem(paint_layer);
    shape_layer = new ShapeLayer();
    shape_layer->setZValue(2);
    addItem(shape_layer);
}

void Paint_area::reset()
{
    if(pic_layer != NULL)
    {
        pic_layer->reset();
    }
    if(paint_layer != NULL)
    {
        paint_layer->reset();
    }
    if(shape_layer != NULL)
    {
        shape_layer->reset();
    }
    if(clip_layer != NULL)
    {
        clip_layer->reset();
    }
    is_save = false;
}

void Paint_area::setPic(QPixmap pic, QRect rect)
{
    setSceneRect(0, 0, rect.width()*2, rect.height()*2);
    pic_layer->setPos(rect.width() / 2, rect.height() / 2);
    pic_layer->setPixmap(pic);
    shape_layer->setPic(pic);
    for(QColor color : History::instance()->get_color())
    {
        pic_layer->setSaveDisableColor(-1, color);
    }
    setSceneRect(0, 0, rect.width() * 2, rect.height() * 2);
}

void Paint_area::setClipPic(QPixmap pix)
{
    clip_layer->setPic(pix);
}

void Paint_area::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    sendEvent(paint_layer, event);
    sendEvent(shape_layer, event);
    if(is_clip)
        sendEvent(clip_layer, event);
    QGraphicsScene::mouseMoveEvent(event);
}

void Paint_area::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    sendEvent(paint_layer, event);
    sendEvent(shape_layer, event);
    if(is_clip)
        sendEvent(clip_layer, event);
    if(event->button() == Qt::BackButton)
    {
        Recorder::instance()->back();
    }
    else if(event->button() == Qt::ForwardButton)
    {
        Recorder::instance()->forward();
    }
    QGraphicsScene::mousePressEvent(event);
}

void Paint_area::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    sendEvent(paint_layer, event);
    sendEvent(shape_layer, event);
    if(is_clip)
        sendEvent(clip_layer, event);
    QGraphicsScene::mouseReleaseEvent(event);
}

void Paint_area::deleteShape()
{
    shape_layer->deleteShape();
}

void Paint_area::paintShape(SHAPE_TYPE type)
{
    shape_type = type;
    shape_layer->setShape(type);
    stateChange(SHAPE);
}

void Paint_area::setOtherLayer()
{
    if(paint_layer == NULL && pic_layer != NULL)
    {
        paint_layer = new Paint_layer(pic_layer);
    }
    else if(paint_layer != NULL && pic_layer != NULL)
    {
        paint_layer->setParentItem(pic_layer);
    }
    if(shape_layer == NULL && pic_layer != NULL)
    {
        shape_layer = new ShapeLayer(pic_layer);
    }
    else if(shape_layer != NULL && pic_layer != NULL)
    {
        shape_layer->setParentItem(pic_layer);
    }
}

void Paint_area::stateChange(PAINT_STATE state)
{
    if(state == this->state)
    {
        return;
    }
    // 放弃当前状态函数
    switch(this->state)
    {
    case ERASE:
        paint_layer->setErase(false);
        break;
    case PAINT:
        paint_layer->setEnableDraw(false);
        break;
    case ARROW:
        if(!is_clip)
            pic_layer->setEnableMove(false);
        shape_layer->setGrabFocus(false);
        break;
    case SHAPE:
        shape_layer->setEnable(false);
        break;
    case MASK:
        break;
    }
    this->state = state;

    switch(state)
    {
    case PAINT:
        paint_layer->setEnableDraw(true);
        break;
    case ERASE:
        paint_layer->setErase(true);
        break;
    case ARROW:
        if(!is_clip)
            pic_layer->setEnableMove(true);
        shape_layer->setGrabFocus(true);
        break;
    case SHAPE:
        shape_layer->setEnable(true);
        break;
    }
}

void Paint_area::initSettingPanel()
{
    connect(Paint_setting_panel::instance(), &Paint_setting_panel::disable_color_change, this,
            [=](int index, QColor color=QColor()){
        if(pic_layer != NULL)
        {
            pic_layer->setDisableColor(index, color);
        }
    });
    connect(Paint_setting_panel::instance(), &Paint_setting_panel::saveDisableColorChange, this,
            [=](int index, QColor color=QColor()){
       if(pic_layer != NULL)
       {
           pic_layer->setSaveDisableColor(index, color);
       }
    });
    connect(Paint_setting_panel::instance(), &Paint_setting_panel::layer_rename, this, [=](int index, QString after_name){

    });
    connect(Paint_setting_panel::instance(), &Paint_setting_panel::remove_layer, this, [=](int index){

    });
    connect(Paint_setting_panel::instance(), &Paint_setting_panel::change_layer_position,
            this, [=](int before_index, int after_index){

    });
    connect(Paint_setting_panel::instance(), &Paint_setting_panel::requestLayersName, this, [=](){
    });
}

bool Paint_area::save(History_data::save_type type, QString path)
{
    if(path == "")
    {
        return false;
    }
    prepareSave();
    is_save = true;
    QRectF bound;
    if(pic_layer != NULL)
    {
        bound = pic_layer->boundingRect();
        bound.moveTo(pic_layer->pos() + bound.topLeft());
    }
    else
    {
        bound = clip_layer->getClipRect();
    }
    bound = bound.united(paint_layer->childrenBoundingRect());
    bound = bound.united(shape_layer->childrenBoundingRect());

    if(bound == QRectF(0, 0, 0, 0))
        return false;

    cv::Mat ans(bound.height(), bound.width(), CV_8UC4);
    for(int i=0; i<bound.height(); i+=32700)
    {
        int height = bound.height() - i > 32700 ? 32700 : bound.height() - i;
        QRect temp_rect(bound.left(), bound.top()+i, bound.width(), height);
        QImage image(bound.width(), height, QImage::Format_ARGB32);
//        image.fill(Qt::transparent);
        QPainter painter(&image);
        render(&painter, QRectF(QPointF(0, 0), image.size()), temp_rect);
        cv::Mat temp_mat = Image_helper::QImage2Mat(image);
        temp_mat.copyTo(ans(cv::Rect(0, i, bound.width(), height)));
    }
    cv::imwrite(path.toLocal8Bit().toStdString(), ans);
    History::instance()->log(type, path);
    endSave();
    return true;
}

bool Paint_area::save2Clipboard()
{
    prepareSave();
    QRectF bound;
    if(pic_layer != NULL)
    {
        bound = pic_layer->boundingRect();
        bound.moveTo(pic_layer->pos() + bound.topLeft());
    }
    else
    {
        bound = clip_layer->getClipRect();
    }
    bound = bound.united(paint_layer->childrenBoundingRect());
    bound = bound.united(shape_layer->childrenBoundingRect());

    if(bound == QRectF(0, 0, 0, 0))
        return false;

    QImage image(bound.width(), bound.height(), QImage::Format_ARGB32);
    image.fill(Qt::transparent);
    QPainter painter(&image);
    render(&painter, QRectF(QPointF(0, 0), bound.size()), bound);
    QClipboard *clip=QApplication::clipboard();
    clip->setImage(image);
    endSave();
    return true;
}

bool Paint_area::needSave()
{
    return pic_layer != NULL && pic_layer->containsPicture() && !is_save;
}

void Paint_area::prepareSave()
{
    if(pic_layer != NULL)
        pic_layer->prepareSave();
    else
        clip_layer->prepareSave();
    shape_layer->prepareSave();
    update();
}

void Paint_area::endSave()
{
    if(pic_layer != NULL)
        pic_layer->endSave();
    else
        clip_layer->endSave();
    shape_layer->endSave();
    update();
}

void Paint_area::sendRequestImage()
{
    if(clip_layer != NULL)
        clip_layer->sendRequestImage();
}
