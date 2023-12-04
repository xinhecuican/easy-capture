#include "rootlayer.h"
#include "../paintarea.h"

RootLayer::RootLayer(PaintArea* area, QGraphicsItem* parent) : QGraphicsItem(parent)
{
    area->addItem(this);
}

QRectF RootLayer::boundingRect() const{
    return QRectF();
}

void RootLayer::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget){

}

void RootLayer::mousePressEvent(QGraphicsSceneMouseEvent *event){

}

void RootLayer::mouseMoveEvent(QGraphicsSceneMouseEvent *event){

}

void RootLayer::mouseReleaseEvent(QGraphicsSceneMouseEvent *event){

}
