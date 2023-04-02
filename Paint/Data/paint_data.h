#ifndef PAINT_DATA_H
#define PAINT_DATA_H
#include<QColor>

struct PaintData {
public:
    PaintData() {};
    PaintData(QColor color, double width, QColor back_color=QColor(255, 255, 255),
              Qt::PenCapStyle cap_style=Qt::SquareCap,
              Qt::PenJoinStyle join_style=Qt::RoundJoin) {
        this->color = color;
        this->width = width;
        this->cap_style = cap_style;
        this->join_style = join_style;
        this->back_color = back_color;
    }
    void copy_to(PaintData* data) {
        data->color = color;
        data->width = width;
        data->cap_style = cap_style;
        data->join_style = join_style;
        data->back_color = back_color;
    }
    QColor color;
    int width;//笔的粗细
    Qt::PenCapStyle cap_style;
    Qt::PenJoinStyle join_style;
    QColor back_color;
};

#endif // PAINT_DATA_H
