#include "picture_layer.h"
#include "Paint/Widgets/recorder.h"
#include<QBitmap>

Picture_layer::Picture_layer()
{

}

Picture_layer::Picture_layer(QString name, QPixmap picture, QRect rect, QWidget* parent) : QWidget(parent)
{
    this->name = name;
    this->picture = picture;
    this->parent = parent;
    now_pos = 0;
    paint_layer = new Paint_layer(parent, "");
    bound = QRect(rect.width()/2, rect.height()/2, rect.width(), rect.height());
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
                node now;
                now.index = i;
                now.dx = dx;
                now.dy = dy;
                int list_size = pos_change_list.size();
                if(now_pos != list_size)
                {
                    for(int i=now_pos; i<list_size; i++)
                    {
                        pos_change_list.removeLast();
                    }
                }
                pos_change_list.append(now);
                now_pos++;
                Recorder::instance()->record(-1, this);
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
    qDebug() << 1;
    delete paint_layer;
    buttons.clear_all();
    picture = QPixmap();
}

void Picture_layer::paint(QPainter* painter, QList<QColor> disable_color, bool is_save)
{
    paint_pic(painter, disable_color, is_save);
    paint_layer->paint(painter, disable_color, is_save);
}

void Picture_layer::erase_and_paint(QPoint point, QPainter* painter, QList<QColor> disable_color)
{
    paint_pic(painter, disable_color, false);
    paint_layer->erase_and_paint(point, painter, disable_color);
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
    }
}

void Picture_layer::set_name(QString name)
{
    this->name = name;
}

int Picture_layer::add_data(Paint_data *style, QPainterPath path)
{
    return paint_layer->add_data(style, path);
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
    return temp.united(paint_layer->bounded_rect());
}

bool Picture_layer::undo(int index)
{
    if(index == -1)
    {
        if(now_pos > 0)
        {
            node now = pos_change_list[now_pos-1];
            now_pos--;
            buttons[Stretch_button::direction(now.index)]->translate(-now.dx, -now.dy);
            on_button_move(Stretch_button::direction(now.index), -now.dx, -now.dy);
        }
    }
    else
    {
        paint_layer->undo(index);
    }
    return true;
}

bool Picture_layer::redo(int index)
{
    if(index == -1)
    {
        if(now_pos < pos_change_list.size())
        {
            node now = pos_change_list[now_pos];
            now_pos++;
            buttons[Stretch_button::direction(now.index)]->translate(now.dx, now.dy);
            on_button_move(Stretch_button::direction(now.index), now.dx, now.dy);
        }
    }
    else
    {
        paint_layer->redo(index);
    }
    return true;
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
