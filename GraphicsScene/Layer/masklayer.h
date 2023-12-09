#ifndef MASKLAYER_H
#define MASKLAYER_H
#include "cliplayerbase.h"
#include "../Widgets/clipregion.h"
#include "../Widgets/colorpicker.h"

class MaskLayer : public ClipLayerBase
{
    Q_OBJECT
public:
    MaskLayer(const QString& name,
              ILayerControl* manager,
              QGraphicsItem* parent=nullptr);
    QRectF boundingRect() const override;
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent* event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = nullptr) override;
    void reset() override;
    QRectF getClipRect() override;
    void onImageSet() override;
    void regionChangeBegin(const QPointF& point);
    void regionMove(const QPointF& point);
    void regionChanged();
    void clearRegion();
    int getRegionCount();
    void prepareSave() override;
    void endSave() override;
    QPainterPath getClipPath() override;
    int getZValue() const override;
    void setEnable(bool enable, int index=0) override;
    int type() const override;
private:
    void addRegion(const QPolygonF& polygon);

private:
    QPainterPath path;
    QList<ClipRegion*> regions;
    QList<ClipRegion*> free_regions;
    QRectF bound;
    QPainterPath freeCapturePath;
    QPointF beginPoint;
    QPointF endPoint;
    bool isDrag;
    bool beginClip;
    Qt::MouseButton button;
    PaintData borderData;
    QColor backgroundColor;
    ColorPicker* colorPicker;
};

#endif // MASKLAYER_H
