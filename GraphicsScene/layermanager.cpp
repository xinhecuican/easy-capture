#include "layermanager.h"
#include <QDebug>

LayerManager::LayerManager()
{
    focusLayer = NULL;
}

void LayerManager::addLayer(LayerBase *layer){
    if(layers.contains(layer->getName())){
        qWarning() << "layer name: " << layer->getName() << " already in manager";
    }
    layers.insert(layer->getName(), layer);
    changeFocusLayer(layer);
}

void LayerManager::changeFocusLayer(const QString &name){
    auto layer = layers.find(name);
    if(layer != layers.end()){
        changeFocusLayer(layer.value());
    }
}

void LayerManager::changeFocusLayer(LayerBase* layer){
    if(focusLayer == layer){
        return;
    }
    LayerBase* beforeFocusLayer = focusLayer;
    focusLayer = layer;
    focusLayer->getFocus();
    if(beforeFocusLayer != NULL){
        beforeFocusLayer->loseFocus();
    }
}

bool LayerManager::hasFocus(LayerBase *layer){
    return focusLayer == layer;
}

LayerBase* LayerManager::removeLayer(const QString &name){
    auto layer = layers.find(name);
    if(layer != layers.end()){
        if(focusLayer == layer.value()){
            focusLayer = NULL;
        }
        LayerBase* value = layer.value();
        layers.remove(name);
        return value;
    }
    return NULL;
}

QList<LayerBase*> LayerManager::getLayers() const{
    return layers.values();
}

LayerBase* LayerManager::getFocusLayer(){
    return focusLayer;
}
