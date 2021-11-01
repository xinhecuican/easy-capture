#ifndef MATH_H
#define MATH_H
#include<QPoint>
#include<QRect>
#include<cmath>
#include "Helper/common.h"
using namespace std;

class Math
{
public:
    static QPoint scroll(float angle, QPoint point, QPoint begin_point);
    static MRect scroll(float angle, QRect rect);
};

#endif // MATH_H
