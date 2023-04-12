#ifndef MASKLAYER_H
#define MASKLAYER_H
#include <QGraphicsObject>
#include <QPainterPath>
#include "Paint/Widgets/Layers/baselayer.h"
#include "clipregion.h"
#include <QPixmap>

class MaskLayer : public QGraphicsObject, public BaseLayer {
    Q_OBJECT
public:
    MaskLayer(QGraphicsItem* parent);
    QPainterPath shape()const override;
    QRectF boundingRect() const override;
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent* event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = nullptr) override;
    void setEnable(bool enable);
    void prepareSave();
    void endSave();
    void reset();
    void clearRegion();
    int getRegionCount();
    QRectF getClipRect();
    QPainterPath getPath();
signals:
    void regionChanged();
private:
    void addRegion(QPolygonF polygon);
    QPainterPath path;
    QList<ClipRegion*> regions;
    QList<ClipRegion*> free_regions;
    QRectF screen_rect;
    Qt::MouseButton button;
    QPointF begin_point;
    QPointF end_point;
    bool is_drag;
    bool begin_clip;
    bool is_save;
    bool is_enable;
    QPixmap mask_pix;
    QPainterPath free_capture_path;
    PaintData borderData;
    QColor backgroundColor;
};

#endif // MASKLAYER_H
