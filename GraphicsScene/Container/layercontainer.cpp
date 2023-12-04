#include "layercontainer.h"
#include "../paintarea.h"

LayerContainer::LayerContainer(PaintArea* area):
    QObject(area),
    area(area),
    widget(NULL){

}
