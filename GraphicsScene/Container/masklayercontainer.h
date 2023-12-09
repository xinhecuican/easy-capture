#ifndef MASKLAYERCONTAINER_H
#define MASKLAYERCONTAINER_H
#include "layercontainer.h"
#include "../Layer/masklayer.h"

class MaskLayerContainer : public LayerContainer
{
public:
    MaskLayerContainer(PaintArea* area);
    QWidget * onValid(QWidget *widgetParent) override;
private:
    MaskLayer* maskLayer;
};

#endif // MASKLAYERCONTAINER_H
