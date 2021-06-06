#ifndef MY_MATH_H
#define MY_MATH_H
#include<QPoint>
#include<QtMath>
#include<QDebug>


class My_math
{
public:
    My_math();
    static QPointF normalize(int x, int y);
};

#endif // MY_MATH_H
