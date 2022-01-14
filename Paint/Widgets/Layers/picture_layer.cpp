#include "picture_layer.h"
#include "Paint/Widgets/recorder.h"
#include<QBitmap>
#include "Paint/Widgets/Recorder_element/resize_record.h"

Picture_layer::Picture_layer()
{

}

Picture_layer::Picture_layer(QString name, QPixmap picture, QRect rect, QWidget* parent) : Ilayer(parent)
{
    this->name = name;
    this->picture = picture;
    this->parent = parent;
    now_pos = 0;
    bound = QRect(rect.width()/2, rect.height()/2, rect.width(), rect.height());
    is_base_layer = false;
    button_group = new Button_group(bound, parent, this);
    button_group->set_constraints(bound);
//    for(int i=0; i<4; i++)
//    {
//        Stretch_button* button = new Stretch_button(direction(i), parent);
//        button->move(i == 1 || i == 2 ? bound.right()-Stretch_button::OFFSET : bound.left()-Stretch_button::OFFSET,
//                     i == 2 || i == 3 ? bound.bottom()-Stretch_button::OFFSET : bound.top()-Stretch_button::OFFSET);
//        button->set_constraints(bound);
//        connect(button, &Stretch_button::button_move, this, &Picture_layer::on_button_move);
//        connect(button, &Stretch_button::button_click, this, [=](bool is_enter, int dx, int dy){
//            if(!is_enter)
//            {
//                Resize_record* record = new Resize_record(this, i, dx, dy);
//                Recorder::instance()->record(record);
//                parent->update();
//            }
//        });
//        buttons.append(button);
//    }
    connect(button_group, &Button_group::button_move, this, &Picture_layer::on_button_move);
    pic_rect = QRect(0, 0, rect.width(), rect.height());
    show();
}

Picture_layer::~Picture_layer()
{
    button_group->deleteLater();
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
        button_group->show_buttons();
    }
    else
    {
        button_group->hide_buttons();
    }
}

void Picture_layer::main_layer_paint(QPainter *painter, QList<QColor> disable_color, bool is_save, int scroll_pos)
{

    if(!is_save)
    {
        int parent_height = parent->parentWidget()->height();
        int h_pos = scroll_pos - parent_height;
        int height = h_pos + 3 * parent_height;
        h_pos = h_pos < bound.top() + pic_rect.top() ? bound.top() + pic_rect.top() : h_pos;
        height = height < bound.top() + pic_rect.bottom() ? height : bound.top() + pic_rect.bottom();
        QImage temp = picture.copy(QRect(pic_rect.left(), h_pos-bound.top(), pic_rect.width(), height-h_pos)).toImage();
        if(height > 0 && h_pos >= bound.top() && h_pos <= bound.bottom())
        {
            for(int i=0; i<disable_color.size(); i++)//设置透明色
            {
                QImage mask = temp.createMaskFromColor(disable_color[i].rgb(), Qt::MaskOutColor);
                temp.setAlphaChannel(mask);
            }
            painter->drawPixmap(bound.left() + pic_rect.left(), h_pos, QPixmap::fromImage(temp));
        }
        QPen pen(QColor(100, 100, 255));
        painter->setPen(pen);
        painter->drawRect(QRect(bound.topLeft()+pic_rect.topLeft(),
                               QSize(pic_rect.width(), pic_rect.height())));
        button_group->show_buttons();
    }
    else
    {
        QImage temp = picture.copy(pic_rect).toImage();
        for(int i=0; i<disable_color.size(); i++)//设置透明色
        {
            QImage mask = temp.createMaskFromColor(disable_color[i].rgb(), Qt::MaskOutColor);
            temp.setAlphaChannel(mask);
        }
        painter->drawPixmap(bound.topLeft() + pic_rect.topLeft(), QPixmap::fromImage(temp));
        button_group->hide_buttons();
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

QPolygon Picture_layer::bounded_rect()
{
    QRect temp = bound.translated(pic_rect.x(), pic_rect.y());
    temp.setWidth(pic_rect.width());
    temp.setHeight(pic_rect.height());
    QPolygon ans;
    ans = ans.united(temp);
    return ans;
}

QString Picture_layer::get_name()
{
    return name;
}

void Picture_layer::on_button_move(direction dir, int dx, int dy)
{
    QRect temp = bounded_rect().boundingRect();
    temp.translate(-8, -8);
    temp.setWidth(temp.width() + 16);
    temp.setHeight(temp.height() + 16);
    switch (dir)
    {
    case NE:pic_rect.setTopRight(pic_rect.topRight() + QPoint(dx, dy));break;
    case NW:pic_rect.setTopLeft(pic_rect.topLeft() + QPoint(dx, dy));break;
    case SE:pic_rect.setBottomRight(pic_rect.bottomRight() + QPoint(dx, dy));break;
    case SW:pic_rect.setBottomLeft(pic_rect.bottomLeft() + QPoint(dx, dy));break;
    }
    parent->update(temp);
}

void Picture_layer::on_size_change(int index, int dx, int dy)
{
    button_group->move_button(index, dx, dy);
    on_button_move((direction)index, dx, dy);
    QRect rect = bounded_rect().boundingRect();
    rect.setTopLeft(rect.topLeft() - QPoint(3, 3));
    rect.setBottomRight(rect.bottomRight() + QPoint(3, 3));
    parent->update(rect);
}

