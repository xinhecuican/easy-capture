#ifndef PAINT_DATA_H
#define PAINT_DATA_H
#include<QColor>

struct Paint_data
{
public:
    Paint_data(){};
    Paint_data(QColor color, double width, Qt::PenCapStyle cap_style=Qt::SquareCap,
               Qt::PenJoinStyle join_style=Qt::BevelJoin)
    {
        this->color = color;
        this->width = width;
        this->cap_style = cap_style;
        this->join_style = join_style;
    }
    void copy_to(Paint_data* data)
    {
        data->color = color;
        data->width = width;
    }
    QColor color;
    int width;//笔的粗细
    Qt::PenCapStyle cap_style;
    Qt::PenJoinStyle join_style;
};

#endif // PAINT_DATA_H
