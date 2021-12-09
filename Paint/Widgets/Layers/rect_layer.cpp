#include "rect_layer.h"
#include<QPen>

Rect_layer::Rect_layer(QWidget* parent, QRect rect) : Ilayer(parent)
{
    this->parent = parent;
    this->bound = rect;
    group = new Button_group(rect, parent, this);
    connect(group, &Button_group::button_move, this, [=](int index, int dx, int dy)
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
    has_focus = true;
}

Rect_layer::~Rect_layer()
{
    group->deleteLater();
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

    if(inner_rect.width() > 20 && inner_rect.height() > 20)
    {
        inner_rect.setTopLeft(inner_rect.topLeft() + QPoint(20, 20));
        inner_rect.setBottomRight(inner_rect.bottomRight() + QPoint(-20, -20));
        polygon = polygon.subtracted(QPolygon(inner_rect));
    }

    return polygon;
}

bool Rect_layer::focuseable()
{
    return true;
}

void Rect_layer::get_focus()
{
    has_focus = true;
    group->show_buttons();
    QRect temp = bound;
    temp = temp.united(bound);
    temp.setTopLeft(temp.topLeft() + QPoint(-10, -10));
    temp.setBottomRight(temp.bottomRight()+QPoint(10, 10));
    parent->update(temp);
}

void Rect_layer::lose_focus()
{
    has_focus = false;
    group->hide_buttons();
    QRect temp = bound;
    temp = temp.united(bound);
    temp.setTopLeft(temp.topLeft() + QPoint(-10, -10));
    temp.setBottomRight(temp.bottomRight()+QPoint(10, 10));
    parent->update(temp);
}

void Rect_layer::mouse_move(int dx, int dy)
{
    if(is_enter)
    {
        group->translate(dx, dy);
        QRect temp = bound;
        bound.translate(dx, dy);
        temp = temp.united(bound);
        temp.setTopLeft(temp.topLeft() + QPoint(-10, -10));
        temp.setBottomRight(temp.bottomRight()+QPoint(10, 10));
        parent->update(temp);
    }
}

void Rect_layer::mouse_enter(int key_code)
{
    if(key_code == Qt::LeftButton)
    {
        is_enter = true;
    }
}

void Rect_layer::mouse_release()
{
    is_enter = false;
}
