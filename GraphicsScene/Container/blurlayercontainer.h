#ifndef BLURLAYERCONTAINER_H
#define BLURLAYERCONTAINER_H
#include "layercontainer.h"
#include "../Layer/blurlayer.h"
#include "../Layer/grabcutlayer.h"

class BlurLayerContainer : public LayerContainer
{
public:
    BlurLayerContainer(PaintArea* area);
    void layerMousePressEvent(QGraphicsSceneMouseEvent *event) override;
    void layerMouseMoveEvent(QGraphicsSceneMouseEvent *event) override;
    void layerMouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;
    QWidget * onValid(QWidget *widgetParent) override;
private:
    BlurLayer* blurLayer;
    GrabcutLayer* grabcutLayer;
    int type;
    int windowSize;
};

#endif // BLURLAYERCONTAINER_H
