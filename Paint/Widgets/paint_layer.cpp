#include "paint_layer.h"
#include<QDir>
#include<QPainter>
#include<QDebug>
#include<QWidget>
#include "recorder.h"

Paint_layer::Paint_layer()
{

}

Paint_layer::Paint_layer(QWidget* parent, QString name) : QWidget(parent)
{
    this->parent = parent;
    this->name = name;
    this->pic = QPicture();
    now_index = 0;
    data = QHash<int, paint_info>();
    delete_data = QHash<int, paint_info>();
    show();
}

void Paint_layer::paint(QImage& image, bool is_save, QRect rect)
{
    QPainter painter(&image);
    if(rect.left() != -1 || rect.top() != -1)
    {
        painter.setClipRect(rect);
    }
    painter.setRenderHints(QPainter::SmoothPixmapTransform | QPainter::Antialiasing, true);
    for(paint_info path : data)
    {
        QPen pen;
        pen.setColor(path.style_info->color);
        pen.setWidth(path.style_info->width);
        pen.setCapStyle(path.style_info->cap_style);
        pen.setJoinStyle(path.style_info->join_style);
        painter.setPen(pen);
        painter.drawPath(path.path);
    }
}

void Paint_layer::erase_and_paint(QPoint point, QImage& image, QRect rect)
{
    QPainter painter;
    QImage temp = image.copy();
    temp.fill(Qt::transparent);
    QImage prototype = temp.copy();
    QRgb rgb = temp.pixel(point.x(), point.y());
    for(auto iter=data.begin(); iter!=data.end();)
    {
        painter.begin(&temp);
        QPen pen;
        pen.setColor(iter->style_info->color);
        pen.setWidth(iter->style_info->width);
        pen.setCapStyle(iter->style_info->cap_style);
        pen.setJoinStyle(iter->style_info->join_style);
        painter.setPen(pen);
        painter.drawPath(iter->path);
        painter.end();
        if(temp.pixel(point.x(), point.y()) != rgb)
        {
            delete_data[iter.key()] = iter.value();
            Recorder::instance()->record(iter.key(), this);
            iter = data.erase(iter);
        }
        else
        {
            iter++;
        }
        temp = prototype.copy();
    }
    paint(image);
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

bool Paint_layer::undo(int index)
{
    if(data.find(index) != data.end())
    {
        delete_data[index] = data[index];
        data.remove(index);
        parent->update();
        return true;
    }
    else if(delete_data.find(index) != delete_data.end())
    {
        data[index] = delete_data[index];
        delete_data.remove(index);
        parent->update();
        return true;
    }
    else
    {
        return false;
    }
}

bool Paint_layer::redo(int index)
{
    if(data.find(index) != data.end())
    {
        delete_data[index] = data[index];
        data.remove(index);
        parent->update();
        return true;
    }
    else if(delete_data.find(index) != delete_data.end())
    {
        data[index] = delete_data[index];
        delete_data.remove(index);
        parent->update();
        return true;
    }
    else
    {
        return false;
    }
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
