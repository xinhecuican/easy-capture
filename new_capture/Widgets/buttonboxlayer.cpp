#include "buttonboxlayer.h"

ButtonBoxLayer::ButtonBoxLayer(QGraphicsItem* parent) : QGraphicsObject(parent)
{

}

QPainterPath ButtonBoxLayer::shape() const
{
    QPainterPath path;
    path.addRect(bound);
    return path;
}

QRectF ButtonBoxLayer::boundingRect() const
{
    return bound;
}

void ButtonBoxLayer::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{

}

void ButtonBoxLayer::hideNormal()
{

}

void ButtonBoxLayer::showNormal()
{

}
