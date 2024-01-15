#ifndef PAINT_DATA_H
#define PAINT_DATA_H
#include<QColor>
#include <QMetaType>

struct PaintData {
public:
    PaintData() {};
    PaintData(QColor color, double width, QColor back_color=QColor(255, 255, 255),
              Qt::PenCapStyle cap_style=Qt::SquareCap,
              Qt::PenJoinStyle join_style=Qt::RoundJoin) {
        this->color = color;
        this->width = width;
        this->capStyle = cap_style;
        this->joinStyle = join_style;
        this->backColor = back_color;
    }
    void copy_to(PaintData* data) {
        data->color = color;
        data->width = width;
        data->capStyle = capStyle;
        data->joinStyle = joinStyle;
        data->backColor = backColor;
    }
    QColor color;
    int width;//笔的粗细
    Qt::PenCapStyle capStyle;
    Qt::PenJoinStyle joinStyle;
    QColor backColor;
};
Q_DECLARE_METATYPE(PaintData)

#endif // PAINT_DATA_H
