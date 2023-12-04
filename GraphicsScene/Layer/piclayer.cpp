#include "piclayer.h"
#include <QDebug>

PicLayer::PicLayer(const QString& name, ILayerControl* manager, bool enableBound, QGraphicsItem* parent) :
    LayerBase(name, manager, parent),
    enableBound(enableBound)
{
    if(enableBound){
        rectLayer = new RectLayer(boundingRect(), "", manager, this);
        rectLayer->setBoundEffect();
        connect(rectLayer, &RectLayer::sizeChange, this, [=]() {
            prepareGeometryChange();
            mask = rectLayer->boundingRect();
            this->update();
        });
    }
    setAcceptHoverEvents(true);
}

void PicLayer::setParameter(const QString &name, const QVariant &var){

}

QRectF PicLayer::boundingRect() const{
    if(!manager->isImageValid()) return QRectF();
    return mask;
}

QPainterPath PicLayer::shape() const{
    QPainterPath path;
    path.addRect(mask);
    return path;
}

void PicLayer::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget){
    if(manager->isImageValid()){
        painter->drawImage(mask.topLeft(), manager->getImage(), mask);
    }
}

void PicLayer::onImageSet(){
    prepareGeometryChange();
    mask = QRectF(QPointF(0, 0), manager->getImage().size());
    if(enableBound){
        rectLayer->setBounding(mask);
        rectLayer->setLimit(mask);
    }
}

void PicLayer::prepareSave(){
    if(enableBound){
        rectLayer->hide();
    }
}

void PicLayer::endSave(){
    if(enableBound){
        rectLayer->show();
    }
}

void PicLayer::reset(){
    mask = QRectF();
}

void PicLayer::setEnable(bool enable) {
    this->enable = enableBound && enable;
}
