#include "stretch_button.h"
#include<QMouseEvent>
#include<QDebug>

Stretch_button::Stretch_button()
{

}

Stretch_button::Stretch_button(direction dir, QWidget* parent):QPushButton(parent)
{
    this->dir = dir;
    setFixedSize(2 * OFFSET, 2 * OFFSET);
    is_set_constraint = false;
    show();
}

Stretch_button::~Stretch_button()
{
}

void Stretch_button::mouseMoveEvent(QMouseEvent *event)
{
    if(!is_click)
    {
        return;
    }
    int dx = event->globalPos().x() - point.x();
    int dy = event->globalPos().y() - point.y();
    point = event->globalPos();
    int sum1 = this->pos().x() + dx;
    int sum2 = this->pos().y() + dy;
    bool larger_x = sum1 > constraint.right() - OFFSET ? true : false;
    bool larger_y = sum2 > constraint.bottom() - OFFSET? true : false;
    bool small_x = sum1 < constraint.left() - OFFSET? true : false;
    bool small_y = sum2 < constraint.top() - OFFSET? true : false;
    if(is_set_constraint)
    {
        if(larger_x)
        {
            dx = constraint.right() - pos().x() - OFFSET;
        }
        if(larger_y)
        {
            dy = constraint.bottom() - pos().y() - OFFSET;
        }
        if(small_x)
        {
            dx = constraint.left() - pos().x() - OFFSET;
        }
        if(small_y)
        {
            dy = constraint.top() - pos().y() - OFFSET;
        }
    }
    deltax += dx;
    deltay += dy;
    this->move(this->pos().x() + dx, this->pos().y() + dy);
    emit button_move(dir, dx, dy);
}

void Stretch_button::mousePressEvent(QMouseEvent *event)
{
    if(event->button() == Qt::LeftButton)
    {
        is_click = true;
    }
    point = event->globalPos();
    deltax = 0;
    deltay = 0;
    emit button_click(true);
}

void Stretch_button::mouseReleaseEvent(QMouseEvent *event)
{
    emit button_click(false, deltax, deltay);
}

void Stretch_button::set_constraints(QRect rect)
{
    constraint = rect;
    is_set_constraint = true;
}

void Stretch_button::translate(int dx, int dy)
{
    this->move(pos().x() + dx, pos().y() + dy);
}
