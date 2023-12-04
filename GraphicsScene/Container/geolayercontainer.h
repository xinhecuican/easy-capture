#ifndef GEOLAYERCONTAINER_H
#define GEOLAYERCONTAINER_H
#include "layercontainer.h"
#include "../Widgets/PaintData.h"

class GeoLayerContainer : public LayerContainer
{
public:
    const static int RECT = 0;
    const static int ARROW = 1;
    GeoLayerContainer(PaintArea* area);
    QWidget* onValid(QWidget* widgetParent) override;
    void layerMouseMoveEvent(QGraphicsSceneMouseEvent *event) override;
    void layerMousePressEvent(QGraphicsSceneMouseEvent* event) override;
    void layerMouseReleaseEvent(QGraphicsSceneMouseEvent* event) override;
private:
    QPointF beginPoint;
    int geoType;
    PaintData rectPaintData;
    PaintData arrowPaintData;
    int rectId;
    int arrowId;
};

#endif // GEOLAYERCONTAINER_H
