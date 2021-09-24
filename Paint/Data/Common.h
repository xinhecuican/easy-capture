#ifndef COMMON_H
#define COMMON_H
#include "paint_data.h"
#include<QPainterPath>
#include<QFont>
enum shape_type{TEXT, DELETE_SHAPE};
enum paint_state{PAINT, ARROW, SHAPE};
struct font_data
{
    QFont font;
    QColor color;
};
struct paint_node
{
    int len;
    font_data data;
    paint_node* next;
};
struct paint_info
{
    paint_info(){};
    paint_info(Paint_data* style, QPainterPath paint_path)
    {
        style_info = style;
        path = paint_path;
    }
    Paint_data* style_info;
    QPainterPath path;
};
#endif // COMMON_H
