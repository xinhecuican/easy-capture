#ifndef PAINTAREA_H
#define PAINTAREA_H
#include <QGraphicsScene>
#include "ilayercontrol.h"
#include "layermanager.h"
#include <QWidget>
#include "Container/layercontainer.h"
#include "Layer/rootlayer.h"
#include "Layer/cliplayerbase.h"

class PaintArea : public QGraphicsScene, public ILayerControl, public LayerManager
{
    Q_OBJECT
public:
    PaintArea(QWidget* parent=nullptr);
    bool save(SaveType type, const QString &path = "") override;
    void setEraseEnable(bool enable) override;
    void setEnable(bool enable, int index=0) override;
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
    void reset(bool newImage=true);
    QGraphicsItem* getRootLayer();
    QRectF getClipRect() override;
    ClipLayerBase* getClipLayer();
    void record(RecordBase *record) override;
    Recorder* getRecorder();
    QImage getSaveImage();
    QRectF getSaveRect();
    bool isSave();
    void installMouseFilter(std::function<bool(Qt::MouseButton)>const& f);
    void registerMousePressHook(std::function<void(QPointF)>const& f);
    void registerMouseMoveHook(std::function<void(QPointF)>const& f);

signals:
    void recordChange();

private:
    LayerContainer* container;
    bool inLayer;
    bool press;
    bool _save;
    QGraphicsItem* mouseGrabber;
    RootLayer* rootLayer;
    ClipLayerBase* clipLayer;
    Recorder* recorder;
    std::function<bool(Qt::MouseButton)> f;
    QList<std::function<void(QPointF)>> pressFuncs;
    QList<std::function<void(QPointF)>> moveFuncs;
    bool installed;
};

#endif // PAINTAREA_H
