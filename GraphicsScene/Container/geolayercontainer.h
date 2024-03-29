#ifndef GEOLAYERCONTAINER_H
#define GEOLAYERCONTAINER_H
#include "layercontainer.h"
#include "../Widgets/PaintData.h"
#include "../Layer/rectlayer.h"
#include "../Layer/arrowlayer.h"
#include "../../Style_widget/colorwidget.h"
#include <QSpinBox>

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
    void applyData(PaintData data);
private:
    QPointF beginPoint;
    int geoType;
    PaintData rectPaintData;
    PaintData arrowPaintData;
    int rectId;
    int arrowId;
    RectLayer* rectLayer;
    ArrowLayer* arrowLayer;
    ColorWidget* colorWidget;
    QSpinBox* widthButton;
};

#endif // GEOLAYERCONTAINER_H
