#include "picture_layer.h"
#include "recorder.h"

Picture_layer::Picture_layer()
{

}

Picture_layer::Picture_layer(QString name, QPixmap picture, QRect rect, QWidget* parent) : QWidget(parent)
{
    this->name = name;
    this->picture = picture;
    this->parent = parent;
    now_pos = 0;
    begin_point = QPoint(rect.width()/2, rect.height()/2);
    paint_layer = new Paint_layer(parent, "");
    base_rect = rect;
    bound = rect;

}

Picture_layer::~Picture_layer()
{
    delete paint_layer;
}

void Picture_layer::paint(QImage& image)
{
    paint_pic(image);
    paint_layer->paint(image);
}

void Picture_layer::erase_and_paint(QPoint point, QImage &image)
{
    paint_pic(image);
    paint_layer->erase_and_paint(point, image);
}

void Picture_layer::paint_pic(QImage& image)
{
    QPainter painter(&image);
    QPixmap temp = picture.copy(base_rect.x(), base_rect.y(), base_rect.width(), base_rect.height());
    painter.drawPixmap(begin_point.x() + base_rect.x(), begin_point.y() + base_rect.y(), temp);
    painter.end();
}

void Picture_layer::set_name(QString name)
{
    this->name = name;
}

int Picture_layer::add_data(Paint_data *style, QPainterPath path)
{
    return paint_layer->add_data(style, path);
}

QPixmap* Picture_layer::get_pic()
{
    return &picture;
}

bool Picture_layer::test_drag(QPoint point)
{
    start_point = point;
    if(QPoint::dotProduct(point, base_rect.topLeft()) < 64)
    {
        drag_pos = top_left;
        move_begin_point = base_rect.topLeft();
        return true;
    }
    if(QPoint::dotProduct(point, base_rect.topRight()) < 64)
    {
        drag_pos = top_right;
        move_begin_point = base_rect.topRight();
        return true;
    }
    if(QPoint::dotProduct(point, base_rect.bottomLeft()) < 64)
    {
        drag_pos = down_left;
        move_begin_point = base_rect.bottomLeft();
        return true;
    }
    if(QPoint::dotProduct(point, base_rect.bottomRight()) < 64)
    {
        drag_pos = down_right;
        move_begin_point = base_rect.bottomRight();
        return true;
    }
    return false;
}

void Picture_layer::point_move(QPoint point)
{
    QPoint constraint_point;
    switch(drag_pos)
    {
    case top_left: constraint_point = base_rect.topLeft();break;
    case top_right: constraint_point = base_rect.topRight();break;
    case down_left: constraint_point = base_rect.bottomLeft();break;
    case down_right: constraint_point = base_rect.bottomRight();break;
    }

    int dx = point.x() - start_point.x();
    int dy = point.y() - start_point.y();
    start_point = point;
    if(dx + constraint_point.x() < bound.x())
    {
        dx = bound.x() - this->pos().x();
    }
    if(dx + constraint_point.x() > bound.x() + bound.width())
    {
        dx = bound.x() + bound.width() - this->pos().x();
    }
    if(dy + constraint_point.y() < bound.y())
    {
        dy = bound.y() - this->pos().y();
    }
    if(dy + constraint_point.y() > bound.y() + bound.height())
    {
        dy = bound.y() + bound.height() - this->pos().y();
    }
    QPoint temp_point = QPoint(constraint_point.x()+dx, constraint_point.y()+dy);
    switch(drag_pos)
    {
    case top_left: base_rect.setTopLeft(temp_point);break;
    case top_right: base_rect.setTopRight(temp_point);break;
    case down_left: base_rect.setBottomLeft(temp_point);break;
    case down_right: base_rect.setBottomRight(temp_point);break;
    }
}

void Picture_layer::point_end(QPoint point)
{
    node new_node;
    new_node.index = drag_pos;
    new_node.dx = point.x() - move_begin_point.x();
    new_node.dy = point.y() - move_begin_point.y();
    pos_change_list.append(new_node);
    Recorder::instance()->record(-1, this);
}

QRect Picture_layer::bounded_rect()
{
    base_rect.moveTo(base_rect.topLeft() + begin_point);
    return base_rect.united(paint_layer->bounded_rect());
}

bool Picture_layer::undo(int index)
{
    if(index == -1)
    {
        if(now_pos > 0)
        {
            node now = pos_change_list[now_pos-1];
            now_pos--;
            corner_point_move(now.index, -now.dx, -now.dy);
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
            node now = pos_change_list[now_pos-1];
            now_pos++;
            corner_point_move(now.index, now.dx, now.dy);
        }
    }
    else
    {
        paint_layer->redo(index);
    }
    return true;
}

void Picture_layer::corner_point_move(int index, int dx, int dy)
{
    switch(index)
    {
    case top_left: base_rect.setTopLeft(base_rect.topLeft() + QPoint(dx, dy));break;
    case top_right: base_rect.setTopRight(base_rect.topRight() + QPoint(dx, dy));break;
    case down_left: base_rect.setBottomLeft(base_rect.bottomLeft() + QPoint(dx, dy));break;
    case down_right: base_rect.setBottomRight(base_rect.bottomRight() + QPoint(dx, dy));break;
    }
}

QString Picture_layer::get_name()
{
    return name;
}
