#include "math.h"

QPoint Math::scroll(float angle, QPoint point, QPoint begin_point) {
    QPoint v1 = point - begin_point;
    if(angle < 0) {
        angle = 360 - angle;
    }
    float x = v1.x() * cos(angle) - v1.y() * sin(angle);
    float y = v1.y() * cos(angle) + v1.x() * sin(angle);
    return QPoint(x + begin_point.x(), y + begin_point.y());
}

MRect Math::scroll(float angle, QRect rect) {
    QPoint begin_point = rect.center();
    MRect ans;
    ans.topleft = scroll(angle, rect.topLeft(), begin_point);
    ans.topright = scroll(angle, rect.topRight(), begin_point);
    ans.bottomleft = scroll(angle, rect.bottomLeft(), begin_point);
    ans.bottomright = scroll(angle, rect.bottomRight(), begin_point);
    return ans;
}

QRectF Math::buildRect(QPointF point1, QPointF point2) {
    qreal top = point1.y() < point2.y() ? point1.y() : point2.y();
    qreal left = point1.x() < point2.x() ? point1.x() : point2.x();
    qreal bottom = point1.y() < point2.y() ? point2.y() : point1.y();
    qreal right = point1.x() < point2.x() ? point2.x() : point1.x();
    return QRectF(left, top, right-left, bottom-top);
}

QRectF Math::resizeRect(QRectF rect, int height, int width) {
    rect.setTopLeft(rect.topLeft() - QPointF(width, height));
    rect.setBottomRight(rect.bottomRight() + QPointF(width, height));
    return rect;
}
