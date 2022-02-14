#include "shapelayer.h"
#include<QGraphicsSceneMouseEvent>
#include "rect_layer.h"
#include "text_layer.h"
#include<QDebug>
#include "Helper/Reflect.h"

ShapeLayer::ShapeLayer(QGraphicsItem* parent) : QGraphicsObject(parent)
{
    is_press = false;
    is_enable = false;
    is_focus = false;
    focus_item = NULL;
}

void ShapeLayer::reset()
{
    focus_item = NULL;
    QList<QGraphicsItem*> children = childItems();
    for(QGraphicsItem* item: children)
    {
        delete item;
    }
}

void ShapeLayer::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{

}

QRectF ShapeLayer::boundingRect() const
{
    return QRectF();
}

void ShapeLayer::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    begin_point = mapFromScene(event->scenePos());
    if(is_enable)
    {
        is_press = true;
    }
    if(is_focus && focus_item != NULL)
    {
        QGraphicsItem* item = dynamic_cast<QGraphicsItem*>(focus_item);
        QPointF point = item->mapFromParent(mapFromScene(event->scenePos()));
        if(!item->contains(point))
        {
            focus_item_func->loseFocusFunc();
            focus_item = NULL;
        }
    }
}

void ShapeLayer::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    if(is_enable && is_press)
    {
        is_press = false;
        QPointF point = mapFromScene(event->scenePos());
        qreal left = 0, top=0, bottom=0, right=0;
        if(point.x() < begin_point.x())
        {
            left = point.x();
            right = begin_point.x();
        }
        else
        {
            left = begin_point.x();
            right = point.x();
        }

        if(point.y() < begin_point.y())
        {
            top = point.y();
            bottom = begin_point.y();
        }
        else
        {
            top = begin_point.y();
            bottom = point.y();
        }
        QRectF rect(QPointF(left, top), QPointF(right, bottom));
        switch(shape)
        {
        case RECTANGLE:
        {
            Rect_layer* rect_layer = new Rect_layer(this, rect);
            rect_layer->setStyle(Rect_layer::RED);
            rect_layer->setEnableMove(true);
            rect_layer->setEnableSizeChange(true);
            connect(rect_layer, &Rect_layer::requestFocus, this, &ShapeLayer::onRequestFocus);
            rect_layer->getFocusFunc();
            break;
        }
        case TEXT:
        {
            if(childContains(begin_point))
            {
                break;
            }
            Text_layer* text_layer = new Text_layer(rect, this);
            if(focus_item != NULL)
            {
                focus_item_func->loseFocusFunc();
            }
            focus_item = text_layer;
            focus_item_func = text_layer;
            text_layer->getFocusFunc();
            connect(text_layer, &Text_layer::requestFocus, this, &ShapeLayer::onRequestFocus);
            break;
        }
        }
    }
}

void ShapeLayer::setShape(SHAPE_TYPE shape)
{
    this->shape = shape;
}

void ShapeLayer::setEnable(bool enable)
{
    this->is_enable = enable;
    setLayersState();
}

void ShapeLayer::setGrabFocus(bool enable)
{
    is_focus = enable;
    setLayersState();
}

void ShapeLayer::onRequestFocus(BaseLayer* object, QGraphicsObject* object2)
{
    if(is_focus)
    {
        focus_item = object2;
        focus_item_func = object;
        object->getFocusFunc();
    }
}

void ShapeLayer::deleteShape()
{
    if(focus_item != NULL)
    {
        delete focus_item;
        focus_item = NULL;
    }
}

void ShapeLayer::setLayersState()
{
    QList<QGraphicsItem*> items = childItems();
    const char* method_name;
    if(is_enable || is_focus)
    {
        method_name = "showNormal";
    }
    else
    {
        method_name = "hideNormal";
    }
    for(QGraphicsItem* item: items)
    {
        QGraphicsObject* object = qgraphicsitem_cast<QGraphicsObject*>(item);
        Reflect::invokeMethod(object->metaObject()->className(), object, method_name);
    }
}

bool ShapeLayer::childContains(QPointF point)
{
    QList<QGraphicsItem*> items = childItems();
    for(QGraphicsItem* item : items)
    {
        QPointF point1 = item->mapFromParent(point);
        if(item->contains(point1))
        {
            return true;
        }
    }
    return false;
}
