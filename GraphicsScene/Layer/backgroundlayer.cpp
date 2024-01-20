#include "backgroundlayer.h"

BackgroundLayer::BackgroundLayer(const QString& name,
                                 ILayerControl* manager,
                                 QGraphicsItem* parent) :
    LayerBase(name, manager, parent)
{

}

void BackgroundLayer::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget){
    if(!isSaving){
        painter->fillRect(bound, QColor(255, 255, 255));
    }
}

void BackgroundLayer::onImageSet(){
    bound = manager->getImage().rect();
}

void BackgroundLayer::reset(){

}

QRectF BackgroundLayer::boundingRect() const{
    return bound;
}

QRectF BackgroundLayer::getSaveRect(){
    return QRectF();
}

int BackgroundLayer::getZValue() const{
    return -1;
}
