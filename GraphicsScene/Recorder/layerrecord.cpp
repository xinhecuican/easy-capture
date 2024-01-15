#include "layerrecord.h"

LayerRecord::LayerRecord(bool show, LayerBase* layer, int type)
    : RecordBase(type),
      layer(layer),
      show(show)
{

}

void LayerRecord::undo(){
    if(show) layer->hide();
    else layer->show();
    layer->update();
}

void LayerRecord::redo(){
    if(show) layer->show();
    else layer->hide();
    layer->update();
}

void LayerRecord::release(){
    layer->release();
}
