#include "rect_layer.h"
#include<QPen>

Rect_layer::Rect_layer(QWidget* parent, QRect rect) : Ilayer(parent)
{
    this->parent = parent;
    this->bound = rect;
}

void Rect_layer::paint(QPainter *painter, QList<QColor> disable_color, bool is_save)
{
    QPen pen;
    pen.setColor(QColor(161, 47, 47));
    pen.setWidth(3);
    pen.setJoinStyle(Qt::RoundJoin);
    painter->setPen(pen);
    painter->drawRect(bound);
}

QString Rect_layer::get_name()
{
    return name;
}

void Rect_layer::set_name(QString name)
{
    this->name = name;
}

QRect Rect_layer::bounded_rect()
{
    return bound;
}
