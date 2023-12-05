#ifndef BLURLAYERCONTAINER_H
#define BLURLAYERCONTAINER_H
#include "layercontainer.h"
#include "../Layer/blurlayer.h"

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
    int windowSize;
};

#endif // BLURLAYERCONTAINER_H
