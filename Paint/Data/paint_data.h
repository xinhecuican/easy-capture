#ifndef PAINT_DATA_H
#define PAINT_DATA_H
#include<QColor>

struct Paint_data
{
public:
    Paint_data(){};
    void copy_to(Paint_data* data)
    {
        data->color = color;
        data->width = width;
    }
    QColor color;
    double width;//笔的粗细
};

#endif // PAINT_DATA_H
