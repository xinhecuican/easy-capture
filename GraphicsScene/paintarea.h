#ifndef PAINTAREA_H
#define PAINTAREA_H
#include <QGraphicsScene>
#include "ilayercontrol.h"
#include "layermanager.h"
#include <QWidget>
#include "Container/layercontainer.h"
#include "Layer/rootlayer.h"

class PaintArea : public QGraphicsScene, public ILayerControl, public LayerManager
{
public:
    PaintArea(QWidget* parent=nullptr);
    bool save(SaveType type, const QString &path = "") override;
    void setEraseEnable(bool enable) override;
    void setEnable(bool enable) override;
    bool hasFocus(LayerBase *layer) override;
    void requestFocus(LayerBase *layer) override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;
    void addLayer(LayerBase *layer) override;
    LayerBase* removeLayer(const QString &name) override;
    void removeThis(LayerBase *layer) override;
    void setContainer(LayerContainer* container);
    void setImage(const QImage& image);
    void reset();
    QGraphicsItem* getRootLayer();
private:
    LayerContainer* container;
    bool inLayer;
    bool press;
    QGraphicsItem* mouseGrabber;
    RootLayer* rootLayer;
};

#endif // PAINTAREA_H
