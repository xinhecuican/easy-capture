#include "layerbase.h"

LayerBase::LayerBase(const QString& name, ILayerControl* manager, QGraphicsItem* parent) :
    QGraphicsObject(parent),
    name(name),
    manager(manager),
    enable(false),
    isSaving(false)
{

}

bool LayerBase::isEnable(){
    return enable;
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

void LayerBase::setEnable(bool enable, int index){
    this->enable = enable;
}

int LayerBase::getZValue() const {
    return 0;
}

void LayerBase::reset() {
    enable = false;
    isSaving = false;
}
