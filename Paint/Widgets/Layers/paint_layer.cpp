#include "paint_layer.h"
#include<QDir>
#include<QPainter>
#include<QDebug>
#include<QWidget>
#include "Paint/Widgets/recorder.h"

Paint_layer::Paint_layer()
{

}

Paint_layer::Paint_layer(QWidget* parent, QString name) : Ilayer(parent)
{
    this->parent = parent;
    this->name = name;
    now_index = 0;
    data = QHash<int, paint_info>();
    show();
}

void Paint_layer::paint(QPainter* painter, QList<QColor> disable_color, bool is_save)
{
    painter->setRenderHints(QPainter::SmoothPixmapTransform | QPainter::Antialiasing, true);
    for(paint_info path : data)
    {
        QPen pen;
        pen.setColor(path.style_info->color);
        pen.setWidth(path.style_info->width);
        pen.setCapStyle(path.style_info->cap_style);
        pen.setJoinStyle(path.style_info->join_style);
        painter->setPen(pen);
        painter->drawPath(path.path);
    }
}

void Paint_layer::erase_and_paint(QPoint point, QPainter* painter, QList<QColor> disable_color)
{
    QRect rect = QRect(point.x()-3, point.y()-3, 6, 6);
    for(auto iter=data.begin(); iter!=data.end();)
    {
        if(!iter->path.controlPointRect().contains(point))
        {
            iter++;
            continue;
        }
        if(iter->path.intersects(rect))
        {
            iter = data.erase(iter);
        }
        else
        {
            iter++;
        }
    }
    paint(painter, disable_color, false);
}

void Paint_layer::set_name(QString name)
{
    this->name = name;
}

int Paint_layer::add_data(Paint_data* style, QPainterPath path)
{

    data[now_index++] = paint_info(style, path);
    return now_index-1;
}

QRect Paint_layer::bounded_rect()
{
    QRect rect;
    for(auto info : data)
    {
        rect = rect.united(info.path.boundingRect().toRect());
    }
    return rect;
}

QString Paint_layer::get_name()
{
    return name;
}

void Paint_layer::on_paint_change(int index, paint_info info)
{
    if(data.find(index) != data.end())
    {
        data.remove(index);
    }
    else
    {
        data[index] = info;
    }
    parent->update();
}

void Paint_layer::clear()
{
    data.clear();
}
