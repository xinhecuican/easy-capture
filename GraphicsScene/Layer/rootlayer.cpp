#include "rootlayer.h"
#include "../paintarea.h"

RootLayer::RootLayer(PaintArea* area, QGraphicsItem* parent) : QGraphicsItem(parent)
{
    erase = false;
    press = false;
    this->area = area;
    area->addItem(this);
}

QRectF RootLayer::boundingRect() const{
    return QRectF();
}

void RootLayer::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget){

}

void RootLayer::mousePressEvent(QGraphicsSceneMouseEvent *event){
    press = true;
}

void RootLayer::mouseMoveEvent(QGraphicsSceneMouseEvent *event){
    if(press && erase){
        deleteChild(event->scenePos());
    }
}

void RootLayer::mouseReleaseEvent(QGraphicsSceneMouseEvent *event){
    press = false;
}

void RootLayer::setErase(bool enable){
    erase = enable;
}

void RootLayer::deleteChild(const QPointF& point){
    QList<LayerBase*> layers = area->getLayers();
    for(auto& layer : layers){
        QPointF point1 = layer->mapFromParent(point);
        if(layer->contains(point1) && layer->isVisible()){
            layer->onDelete(point1);
        }
    }
}
