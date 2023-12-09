#include "layerbase.h"

LayerBase::LayerBase(const QString& name, ILayerControl* manager, QGraphicsItem* parent) :
    QGraphicsObject(parent),
    name(name),
    manager(manager),
    erase(false),
    enable(false),
    isSaving(false)
{

}

bool LayerBase::isEnable(){
    return enable;
}

bool LayerBase::isErase(){
    return erase;
}

void LayerBase::prepareSave(){
    isSaving = true;
}

void LayerBase::endSave(){
    isSaving = false;
}

QString LayerBase::getName() const{
    return name;
}

void LayerBase::setErase(bool enable){
    erase = enable;
}

void LayerBase::setEnable(bool enable, int index){
    this->enable = enable;
}

int LayerBase::getZValue() const {
    return 0;
}

void LayerBase::reset() {
    enable = false;
    erase = false;
    isSaving = false;
}
