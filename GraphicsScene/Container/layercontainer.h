#ifndef LAYERCONTAINER_H
#define LAYERCONTAINER_H
#include <QHBoxLayout>
#include <QGraphicsObject>
#include <QGraphicsSceneMouseEvent>
#include <QToolButton>
#include "../ilayercontrol.h"

class PaintArea;

class LayerContainer : public QObject
{
public:
    LayerContainer(PaintArea* area);
    virtual ~LayerContainer(){
    }
    virtual void layerMouseMoveEvent(QGraphicsSceneMouseEvent *event) {}
    virtual void layerMousePressEvent(QGraphicsSceneMouseEvent* event) {}
    virtual void layerMouseReleaseEvent(QGraphicsSceneMouseEvent* event) {}
    virtual QWidget* onValid(QWidget* widgetParent)=0;
protected:
    PaintArea* area;
    QWidget* widget;
};

#endif // LAYERCONTAINER_H
