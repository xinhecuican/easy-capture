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
    QPolygon polygon = QPolygon(rect);
    if(polygon.point(0) != polygon.point(polygon.size()-1))
    {
        polygon.append(polygon.point(0));
    }
    paint_layer = new Paint_layer(parent, "");
    region = new Capture_region(this, parent, polygon);
    base_rect = region->get_polygon().boundingRect();
    region->move(150, 150);
    region->set_constraint(150, 150, 135+rect.width(), 135 + rect.height());
    parent->resize(rect.width()+400, rect.height()+400);
    parent->update();
}

Picture_layer::~Picture_layer()
{
    delete region;
    delete paint_layer;
}

void Picture_layer::paint(QImage& image)
{
    QPainter painter(&image);
    QRect rect = region->get_polygon().boundingRect();
    QPixmap temp = picture.copy(rect.x()-150, rect.y()-150, rect.width(), rect.height());
    painter.drawPixmap(rect.x(), rect.y(), temp);
    painter.end();
    paint_layer->paint(image);
}

void Picture_layer::erase_and_paint(QPoint point, QImage &image)
{
    paint_layer->erase_and_paint(point, image);
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

void Picture_layer::control_point_position_change(int index, QList<int> position, int dx, int dy)
{
    region->point_move(position, dx, dy);
}

void Picture_layer::position_change_begin(int index, QPoint position)
{
    node new_node;
    new_node.index = index;
    new_node.dx = position.x();
    new_node.dy = position.y();

    int len = pos_change_list.size();
    for(int i=now_pos; i<len; i++)
    {
        pos_change_list.removeLast();
    }
    now_pos++;
    pos_change_list.append(new_node);
}

void Picture_layer::position_change_end(QPoint position)
{
    pos_change_list.last().dx = position.x() - pos_change_list.last().dx;
    pos_change_list.last().dy = position.y() - pos_change_list.last().dy;
    Recorder::instance()->record(-1, this);
}

QRect Picture_layer::bounded_rect()
{
    return region->get_polygon().boundingRect();
}

bool Picture_layer::undo(int index)
{
    if(index == -1)
    {
        if(now_pos > 0)
        {
            node now = pos_change_list[now_pos-1];
            now_pos--;
            region->stretch_point_move(now.index, -now.dx, -now.dy);
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
            region->stretch_point_move(now.index, now.dx, now.dy);
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
