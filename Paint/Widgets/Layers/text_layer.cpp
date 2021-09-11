#include "text_layer.h"

Text_layer::Text_layer(QRect bound_rect, QWidget* parent):QWidget(parent)
{
    edit = new QTextEdit(parent);
    edit->setStyleSheet("background:transparent;border-width:0;border-style:outset");
    edit->hide();
    this->bound_rect = bound_rect;
}

void Text_layer::paint(QPainter *painter, QList<QColor> disable_color, bool is_save)
{
    painter->drawText(bound_rect, str);
}

void Text_layer::erase_and_paint(QPoint point, QPainter *painter, QList<QColor> disable_color)
{

}

void Text_layer::mouseDoubleClickEvent(QMouseEvent *event)
{
    if(edit->isHidden())
    {
        edit->resize(bound_rect.width(), bound_rect.height());
        edit->setText(str);
        edit->show();
    }
}

QRect Text_layer::bounded_rect()
{

}

int Text_layer::add_data(Paint_data *style, QPainterPath path)
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
