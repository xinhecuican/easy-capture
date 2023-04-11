#include "paintitem.h"
#include "Helper/math.h"
#include "Helper/debug.h"

PaintItem::PaintItem(PaintInfo info, QGraphicsItem* parent) : QGraphicsObject(parent) {
    this->info = info;
    setCacheMode(ItemCoordinateCache);
    pen.setColor(info.style_info->color);
    pen.setWidth(info.style_info->width);
    pen.setCapStyle(info.style_info->cap_style);
    pen.setJoinStyle(info.style_info->join_style);
    if(info.path.size() != 0) {
        path = QPainterPath();
        path.moveTo(info.path.at(0));
        boundRect.setTopLeft(info.path.at(0));
    }
    _isEnd = false;
}

QRectF PaintItem::boundingRect() const {
    return Math::resizeRect(boundRect, info.style_info->width+1, info.style_info->width+1);
}

void PaintItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) {
    if(!_isEnd) {
        paintLine(painter);
    }
//    for(int i=0; i<info.path.size()-1; i++) {
//        painter->drawLine(info.path.at(i), info.path.at(i+1));
//    }
}

void PaintItem::paintLine(QPainter *painter) {
    painter->setPen(pen);
    painter->drawPath(path);
}

QPainterPath PaintItem::shape() const {
    QPainterPath shape;
    shape.addPolygon(QPolygonF(info.path));
    return shape;
}

void PaintItem::addPoint(QPointF point, bool needUpdate) {
    QPointF lastPoint = info.path.last();
    info.path.append(point);
    this->path.lineTo(point);
    if(boundRect.left() > point.x()) {
        boundRect.setLeft(point.x());
    }
    if(boundRect.right() < point.x()) {
        boundRect.setRight(point.x());
    }
    if(boundRect.top() > point.y()) {
        boundRect.setTop(point.y());
    }
    if(boundRect.bottom() < point.y()) {
        boundRect.setBottom(point.y());
    }
    if(needUpdate)
        update(Math::resizeRect(Math::buildRect(lastPoint, point), info.style_info->width+1, info.style_info->width+1));
}


void PaintItem::setEnd() {
    _isEnd = true;
}
