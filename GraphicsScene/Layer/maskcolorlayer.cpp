#include "maskcolorlayer.h"

MaskColorLayer::MaskColorLayer(const QString& name,
                               ILayerControl* manager,
                               QGraphicsItem* parent)
    : LayerBase(name, manager, parent)
{

}

QRectF MaskColorLayer::boundingRect() const{
    return QRectF();
}

void MaskColorLayer::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget){

}

void MaskColorLayer::reset(){
    colors.clear();
    LayerBase::reset();
}

void MaskColorLayer::setEnable(bool enable, int index){
    this->enable = false;
}

void MaskColorLayer::addMaskColor(const QColor &color){
    colors.append(color);
    manager->maskColor(color);
}

void MaskColorLayer::removeMaskColor(int index){
    colors.removeAt(index);
    manager->resetImage();
    for(QColor color : colors){
        manager->maskColor(color);
    }
}

QList<QColor> MaskColorLayer::getMaskColors(){
    return colors;
}

int MaskColorLayer::type() const{
    return 102409;
}

int MaskColorLayer::getZValue() const{
    return 2;
}
