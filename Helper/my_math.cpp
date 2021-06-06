#include "my_math.h"

My_math::My_math()
{

}

QPointF My_math::normalize(int x, int y)
{
    double d = sqrt(x * x + y * y);

    return QPointF(x / d, y / d);
}
