#include "paintlayer.h"
#include <QtMath>

PaintLayer::PaintLayer(const QString& name,
                       ILayerControl* manager,
                       QGraphicsItem* parent) :
      LayerBase(name, manager, parent)
{
    method = Smooth;
}

QRectF PaintLayer::boundingRect() const {
    return path.boundingRect();
}

QPainterPath PaintLayer::shape() const {
    return path;
}

void PaintLayer::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) {
    painter->save();
    painter->setRenderHint(QPainter::Antialiasing, true);
    painter->drawPath(path);
    painter->restore();
}

void PaintLayer::reset(){
    manager->removeThis(this);
}

void PaintLayer::mousePressEvent(QGraphicsSceneMouseEvent *event){

}

void PaintLayer::addPoint(const QPointF& point){
    if(points.size() > 0){
        QLineF line(point, points.last());
        if(line.length() < 2) return;
    }
    points.append(point);
    switch (method) {
    case Smooth:
        path = SmoothCurveGenerator::generateSmoothCurve(points);
        break;
    case PaintPath:
        if(points.size() == 1){
            path.moveTo(points[0]);
        }
        else{
            path.lineTo(points.last());
        }
        break;
    }
    update();
}

void PaintLayer::setMethod(PaintMethod method) {
    this->method = method;
}

void PaintLayer::setParameter(const QString &name, const QVariant &var) {
    if(name == "color") {
        QColor color = var.value<QColor>();
        pen.setColor(color);
    }
    if(name == "width") {
        pen.setWidth(var.toInt());
    }
}

void PaintLayer::setEnable(bool enable) {
    this->enable = false;
}


QPainterPath SmoothCurveGenerator::generateSmoothCurve(QList<QPointF> points, bool closed, double tension, int numberOfSegments) {
    QList<double> ps;

    foreach (QPointF p, points) {
        ps << p.x() << p.y();
    }

    return SmoothCurveGenerator::generateSmoothCurve(ps, closed, tension, numberOfSegments);
}

QPainterPath SmoothCurveGenerator::generateSmoothCurve(QList<double> points, bool closed, double tension, int numberOfSegments) {
    if(points.size() < 4){
        return QPainterPath();
    }
    QList<double> ps(points); // clone array so we don't change the original points
    QList<double> result; // generated smooth curve coordinates
    double x, y;
    double t1x, t2x, t1y, t2y;
    double c1, c2, c3, c4;
    double st;

    // The algorithm require a previous and next point to the actual point array.
    // Check if we will draw closed or open curve.
    // If closed, copy end points to beginning and first points to end
    // If open, duplicate first points to befinning, end points to end
    if (closed) {
        ps.prepend(points[points.length() - 1]);
        ps.prepend(points[points.length() - 2]);
        ps.prepend(points[points.length() - 1]);
        ps.prepend(points[points.length() - 2]);
        ps.append(points[0]);
        ps.append(points[1]);
    } else {
        ps.prepend(points[1]); // copy 1st point and insert at beginning
        ps.prepend(points[0]);
        ps.append(points[points.length() - 2]); // copy last point and append
        ps.append(points[points.length() - 1]);
    }

    // 1. loop goes through point array
    // 2. loop goes through each segment between the 2 points + 1e point before and after
    for (int i = 2; i < (ps.length() - 4); i += 2) {
        // calculate tension vectors
        t1x = (ps[i + 2] - ps[i - 2]) * tension;
        t2x = (ps[i + 4] - ps[i - 0]) * tension;
        t1y = (ps[i + 3] - ps[i - 1]) * tension;
        t2y = (ps[i + 5] - ps[i + 1]) * tension;

        for (int t = 0; t <= numberOfSegments; t++) {
            // calculate step
            st = (double)t / (double)numberOfSegments;

            // calculate cardinals
            c1 = 2 * qPow(st, 3) - 3 * qPow(st, 2) + 1;
            c2 = -2 * qPow(st, 3) + 3 * qPow(st, 2);
            c3 = qPow(st, 3) - 2 * qPow(st, 2) + st;
            c4 = qPow(st, 3) - qPow(st, 2);

            // calculate x and y cords with common control vectors
            x = c1 * ps[i] + c2 * ps[i + 2] + c3 * t1x + c4 * t2x;
            y = c1 * ps[i + 1] + c2 * ps[i + 3] + c3 * t1y + c4 * t2y;

            //store points in array
            result << x << y;
        }
    }

    // 使用的平滑曲线的坐标创建 QPainterPath
    QPainterPath path;
    path.moveTo(result[0], result[1]);
    for (int i = 2; i < result.length() - 2; i += 2) {
        path.lineTo(result[i+0], result[i+1]);
    }

    if (closed) {
        path.closeSubpath();
    }

    return path;
}
