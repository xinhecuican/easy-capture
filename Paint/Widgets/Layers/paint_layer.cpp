#include "paint_layer.h"
#include<QDir>
#include<QPainter>
#include<QDebug>
#include<QWidget>
#include "Paint/Widgets/recorder.h"
#include "Paint/Widgets/style_manager.h"
#include<QGraphicsSceneMouseEvent>
#include "Paint/Widgets/recorder.h"
#include "Paint/Widgets/Recorder_element/paintdeleterecord.h"

Paint_layer::Paint_layer(QGraphicsItem* parent) : QGraphicsObject(parent)
{
    this->parent = parent;
    is_enable = true;
    is_press = false;
    is_erase = false;
    setAcceptedMouseButtons(Qt::NoButton);
}

Paint_layer::~Paint_layer()
{
    reset();
}

void Paint_layer::reset()
{
    for(QGraphicsItem* line : lines)
    {
        delete line;
    }
    lines.clear();
}

void Paint_layer::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    path = QPainterPath();
    if(event->button() == Qt::LeftButton)
    {
        is_press = true;
    }
    if(is_enable)
    {
        path.moveTo(mapFromScene(event->scenePos()));
        Paint_data* paint_data = Style_manager::instance()->get();
        paint_info info(paint_data, path);
        current_item = new PaintItem(info, this); // 添加一个线条
    }
    if(is_erase)
    {
        removeLines(mapFromScene(event->scenePos()));
    }
}

void Paint_layer::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    if(is_enable && is_press)
    {
        current_item->addPoint(mapFromScene(event->scenePos()));
    }
    if(is_erase && is_press)
    {
        removeLines(mapFromScene(event->scenePos()));
    }
}

void Paint_layer::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    if(is_enable && is_press)
    {
        current_item->addPoint(mapFromScene(event->scenePos()));
        lines.append(current_item);
        path = QPainterPath();
        PaintRecord* record = new PaintRecord(current_item);
        Recorder::instance()->record(record);
    }
    is_press = false;
}

void Paint_layer::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    painter->setRenderHints(QPainter::SmoothPixmapTransform | QPainter::Antialiasing, true);
    Paint_data* paint_data = Style_manager::instance()->get();
    QPen pen;
    pen.setColor(paint_data->color);
    pen.setWidth(paint_data->width);
    pen.setCapStyle(paint_data->cap_style);
    pen.setJoinStyle(paint_data->join_style);
    painter->setPen(pen);
    painter->drawPath(path);
}

void Paint_layer::setEnableDraw(bool enable)
{
    this->is_enable = enable;
}

QRectF Paint_layer::boundingRect() const
{
    return path.boundingRect();
}

void Paint_layer::setErase(bool enable)
{
    is_erase = enable;
}

void Paint_layer::removeLines(QPointF point)
{
    QRectF rect(point - QPointF(3, 3), point + QPointF(3, 3));
    QList<PaintItem*> items;
    for(PaintItem* item : lines)
    {
        if(item->shape().intersects(rect))
        {
            items.append(item);
        }
    }
    for(PaintItem* item: items)
    {
        PaintDeleteRecord* record = new PaintDeleteRecord(this, item, "undoRedoPaintFunc");
        Recorder::instance()->record(record);
        lines.removeOne(item);
    }
    update();
}

void Paint_layer::undoRedoPaintFunc(bool is_undo, PaintItem *item)
{
    if(is_undo)
    {
        item->show();
        lines.append(item);
    }
    else
    {
        item->hide();
        lines.removeOne(item);
    }
}
