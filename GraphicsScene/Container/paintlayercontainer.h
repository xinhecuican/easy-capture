#ifndef PAINTLAYERCONTAINER_H
#define PAINTLAYERCONTAINER_H
#include "layercontainer.h"
#include "../Widgets/PaintData.h"
#include "../Layer/paintlayer.h"

class PaintLayerContainer : public LayerContainer
{
public:
    static const int Pencil = 0;
    static const int Highlighter = 1;
    PaintLayerContainer(PaintArea* area);
    void layerMousePressEvent(QGraphicsSceneMouseEvent *event) override;
    void layerMouseMoveEvent(QGraphicsSceneMouseEvent *event) override;
    void layerMouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;
    QWidget * onValid(QWidget *widgetParent) override;
private:
    int type;
    PaintData pencilData;
    PaintData highlighterData;
    PaintLayer* paintLayer;
    int paintId;
    qint64 beforeTime;
};

#endif // PAINTLAYERCONTAINER_H
