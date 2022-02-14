#include "paintitem.h"

PaintItem::PaintItem(paint_info info, QGraphicsItem* parent) : QGraphicsItem(parent)
{
    this->info = info;
}

QRectF PaintItem::boundingRect() const
{
    return info.path.boundingRect();
}

void PaintItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    painter->setRenderHints(QPainter::SmoothPixmapTransform | QPainter::Antialiasing, true);
    QPen pen;
    pen.setColor(info.style_info->color);
    pen.setWidth(info.style_info->width);
    pen.setCapStyle(info.style_info->cap_style);
    pen.setJoinStyle(info.style_info->join_style);
    painter->setPen(pen);
    painter->drawPath(info.path);
}

void PaintItem::setPath(QPainterPath path)
{
    this->info.path = path;
}

QPainterPath PaintItem::shape() const
{
    return this->info.path;
}
