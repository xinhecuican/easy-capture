#include "picture_layer.h"
#include "Paint/Widgets/recorder.h"
#include<QBitmap>
#include "Paint/Widgets/Recorder_element/resize_record.h"

Picture_layer::Picture_layer()
{

}

Picture_layer::Picture_layer(QString name, QPixmap picture, QRect rect, QWidget* parent) : QWidget(parent)
{
    this->name = name;
    this->picture = picture;
    this->parent = parent;
    now_pos = 0;
    bound = QRect(rect.width()/2, rect.height()/2, rect.width(), rect.height());
    is_base_layer = false;
    for(int i=0; i<4; i++)
    {
        Stretch_button* button = new Stretch_button(Stretch_button::direction(i), parent);
        button->move(i == 1 || i == 2 ? bound.right()-Stretch_button::OFFSET : bound.left()-Stretch_button::OFFSET,
                     i == 2 || i == 3 ? bound.bottom()-Stretch_button::OFFSET : bound.top()-Stretch_button::OFFSET);
        button->set_constraints(bound);
        connect(button, &Stretch_button::button_move, this, &Picture_layer::on_button_move);
        connect(button, &Stretch_button::button_click, this, [=](bool is_enter, int dx, int dy){
            if(!is_enter)
            {
                Resize_record* record = new Resize_record(this, i, dx, dy);
                Recorder::instance()->record(record);
                parent->update();
            }
        });
        buttons.append(button);
    }
    pic_rect = QRect(0, 0, rect.width(), rect.height());
    show();
}

Picture_layer::~Picture_layer()
{
    buttons.clear_all();
    picture = QPixmap();
}

void Picture_layer::paint(QPainter* painter, QList<QColor> disable_color, bool is_save)
{
    paint_pic(painter, disable_color, is_save);
}

void Picture_layer::paint_pic(QPainter* painter, QList<QColor> disable_color, bool is_save)
{
    QImage temp = picture.copy(pic_rect).toImage();
    for(int i=0; i<disable_color.size(); i++)//设置透明色
    {
        QImage mask = temp.createMaskFromColor(disable_color[i].rgb(), Qt::MaskOutColor);
        temp.setAlphaChannel(mask);
    }
    painter->drawPixmap(bound.topLeft() + pic_rect.topLeft(), QPixmap::fromImage(temp));
    if(!is_save)
    {
        QPen pen(QColor(100, 100, 255));
        painter->setPen(pen);
        painter->drawRect(QRect(bound.topLeft()+pic_rect.topLeft(),
                               QSize(pic_rect.width(), pic_rect.height())));
        for(int i=0; i<buttons.size(); i++)
        {
            buttons[i]->show();
        }
    }
    else
    {
        for(int i=0; i<buttons.size(); i++)
        {
            buttons[i]->hide();
        }
    }
}

void Picture_layer::set_name(QString name)
{
    this->name = name;
}

QPixmap Picture_layer::get_pic()
{
    return picture;
}

QRect Picture_layer::bounded_rect()
{
    QRect temp = bound.translated(pic_rect.x(), pic_rect.y());
    temp.setWidth(pic_rect.width());
    temp.setHeight(pic_rect.height());
    return temp;
}

QString Picture_layer::get_name()
{
    return name;
}

void Picture_layer::on_button_move(Stretch_button::direction dir, int dx, int dy)
{
    QRect temp = bounded_rect();
    temp.translate(-8, -8);
    temp.setWidth(temp.width() + 16);
    temp.setHeight(temp.height() + 16);
    switch (dir)
    {
    case Stretch_button::NE:
        buttons[Stretch_button::NW]->translate(0, dy);
        buttons[Stretch_button::SE]->translate(dx, 0);
        pic_rect.setTopRight(pic_rect.topRight() + QPoint(dx, dy));
        break;
    case Stretch_button::NW:
        buttons[Stretch_button::NE]->translate(0, dy);
        buttons[Stretch_button::SW]->translate(dx, 0);
        pic_rect.setTopLeft(pic_rect.topLeft() + QPoint(dx, dy));
        break;
    case Stretch_button::SE:
        buttons[Stretch_button::NE]->translate(dx, 0);
        buttons[Stretch_button::SW]->translate(0, dy);
        pic_rect.setBottomRight(pic_rect.bottomRight() + QPoint(dx, dy));
        break;
    case Stretch_button::SW:
        buttons[Stretch_button::SE]->translate(0, dy);
        buttons[Stretch_button::NW]->translate(dx, 0);
        pic_rect.setBottomLeft(pic_rect.bottomLeft() + QPoint(dx, dy));
        break;
    }
    parent->update(temp);
}

void Picture_layer::on_size_change(int index, int dx, int dy)
{
    this->buttons[index]->translate(dx, dy);
    on_button_move((Stretch_button::direction)index, dx, dy);
    QRect rect = bounded_rect();
    rect.setTopLeft(rect.topLeft() - QPoint(3, 3));
    rect.setBottomRight(rect.bottomRight() + QPoint(3, 3));
    parent->update(rect);
}

