#ifndef MATH_H
#define MATH_H
#include<QPoint>
#include<QRect>
#include<cmath>
#include "common.h"
using namespace std;

class Math {
public:
    static QPoint scroll(float angle, QPoint point, QPoint begin_point);
    static MRect scroll(float angle, QRect rect);
    static QRectF buildRect(QPointF point1, QPointF point2);

    /**
     * @brief resizeRect 将长方形的长和宽变大或变小2 * height
     * @param rect
     * @param height
     * @param width
     * @return
     */
    static QRectF resizeRect(QRectF rect, int height, int width);
};

#endif // MATH_H
