#include "paint_layer.h"
#include<QDir>
#include<QPainter>
#include<QDebug>
#include<QWidget>

Paint_layer::Paint_layer()
{

}

Paint_layer::Paint_layer(QWidget* parent, QString name)
{
    this->parent = parent;
    this->name = name;
    this->pic = QPicture();
    now_index = 0;
    data = QHash<int, paint_info>();
    delete_data = QHash<int, paint_info>();
}

void Paint_layer::paint(QImage &image)
{
    QPainter painter(&image);
    for(paint_info path : data)
    {
        QPen pen;
        pen.setColor(path.style_info->color);
        pen.setWidth(path.style_info->width);
        painter.setPen(pen);
        painter.drawPath(path.path);
    }
}

void Paint_layer::save()
{
    QDir base_dir = QDir("Data/Temp");
    if(!base_dir.exists())
    {
        QDir dir;
        dir.mkdir("Data/Temp");
    }
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

void Paint_layer::undo(int index)
{
    if(data.find(index) == data.end())
    {
        data[index] = delete_data[index];
        delete_data.remove(index);
    }
    else
    {
        delete_data[index] = data[index];
        data.remove(index);
    }
    parent->update();
}

void Paint_layer::redo(int index)
{
    if(data.find(index) == data.end())
    {
        data[index] = delete_data[index];
        delete_data.remove(index);
    }
    else
    {
        delete_data[index] = data[index];
        data.remove(index);
    }
    parent->update();
}
