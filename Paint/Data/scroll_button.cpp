#include "scroll_button.h"
#include<QMouseEvent>

Scroll_button::Scroll_button(QWidget* parent) : QPushButton(parent)
{
    parent->installEventFilter(this);
}

void Scroll_button::mouseMoveEvent(QMouseEvent *event)
{
    emit button_move(this->pos() + event->pos());
}

void Scroll_button::mouseReleaseEvent(QMouseEvent *event)
{
    emit button_release(this->pos() + event->pos());
}

void Scroll_button::mousePressEvent(QMouseEvent *event)
{
    emit button_click(this->pos() + event->pos());
}
