#include "text_layer.h"

Text_layer::Text_layer(QRect bound_rect, QWidget* parent):QWidget(parent)
{
    edit = new QTextEdit(parent);
    edit->setStyleSheet("background:transparent;border-width:0;border-style:outset");
    this->bound_rect = bound_rect;
}

void Text_layer::paint(QPainter *painter, QList<QColor> disable_color, bool is_save)
{

}

void Text_layer::erase_and_paint(QPoint point, QPainter *painter, QList<QColor> disable_color)
{

}

QRect Text_layer::bounded_rect()
{

}

int Text_layer::add_data(Paint_data *style, QPainterPath path)
{

}

bool Text_layer::redo(int index)
{

}

bool Text_layer::undo(int index)
{

}

void Text_layer::set_name(QString name)
{
    this->name = name;
}

QString Text_layer::get_name()
{
    return name;
}

