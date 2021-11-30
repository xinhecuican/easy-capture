#include "rect_layer.h"
#include<QPen>

Rect_layer::Rect_layer(QWidget* parent, QRect rect) : Ilayer(parent)
{
    this->parent = parent;
    this->bound = rect;
    Button_group* group = new Button_group(rect, parent, this);
    connect(group, &Button_group::move_button, this, [=](int index, int dx, int dy)
    {
        QRect temp = bounded_rect().boundingRect();
        temp.translate(-8, -8);
        temp.setWidth(temp.width() + 16);
        temp.setHeight(temp.height() + 16);
        switch (index)
        {
        case NE:bound.setTopRight(bound.topRight() + QPoint(dx, dy));break;
        case NW:bound.setTopLeft(bound.topLeft() + QPoint(dx, dy));break;
        case SE:bound.setBottomRight(bound.bottomRight() + QPoint(dx, dy));break;
        case SW:bound.setBottomLeft(bound.bottomLeft() + QPoint(dx, dy));break;
        }
        parent->update(temp);
    });
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

QPolygon Rect_layer::bounded_rect()
{
    QPolygon polygon = QPolygon(bound);
    QRect inner_rect = bound;
    inner_rect.setTopLeft(inner_rect.topLeft() + QPoint(10, 10));
    inner_rect.setBottomRight(inner_rect.bottomRight() + QPoint(-10, -10));
    polygon = polygon.subtracted(QPolygon(inner_rect));
    return polygon;
}

bool Rect_layer::focuseable()
{
    return true;
}
