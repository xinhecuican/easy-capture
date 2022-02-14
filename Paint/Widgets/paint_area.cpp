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
#include<windows.h>
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

Paint_area::Paint_area(QObject* parent) : QGraphicsScene(parent)
{
    pic_layer = NULL;
    paint_layer = new Paint_layer();
    shape_layer = new ShapeLayer();
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
}

void Paint_area::setPic(QPixmap pic, QRect rect)
{
    if(pic_layer == NULL)
    {
        pic_layer = new Picture_layer();
        setOtherLayer();
    }
    pic_layer->setPos(rect.width() / 2, rect.height() / 2);
    pic_layer->setPixmap(pic);
    this->addItem(pic_layer);
    setFocusItem(pic_layer);
    setSceneRect(0, 0, rect.width() * 2, rect.height() * 2);
}

void Paint_area::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    sendEvent(paint_layer, event);
    QGraphicsScene::mouseMoveEvent(event);
}

void Paint_area::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    sendEvent(paint_layer, event);
    sendEvent(shape_layer, event);
    QGraphicsScene::mousePressEvent(event);
}

void Paint_area::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    sendEvent(paint_layer, event);
    sendEvent(shape_layer, event);
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
        pic_layer->setEnableMove(false);
        shape_layer->setGrabFocus(false);
        break;
    case SHAPE:
        shape_layer->setEnable(false);
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
        //                area->set_disable_color(index, color);
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
