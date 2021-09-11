#ifndef COMMON_H
#define COMMON_H
#include "paint_data.h"
#include<QPainterPath>
enum shape_type{TEXT};
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
