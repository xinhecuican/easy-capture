#ifndef HELPER_COMMON_H
#define HELPER_COMMON_H
#include<QPoint>

struct MRect
{
    QPoint topleft;
    QPoint topright;
    QPoint bottomleft;
    QPoint bottomright;
    MRect(){};
};

#endif // COMMON_H
