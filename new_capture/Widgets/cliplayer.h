#ifndef CLIPLAYER_H
#define CLIPLAYER_H
#include <QGraphicsObject>
#include "Paint/Widgets/Layers/baselayer.h"
#include "clipregion.h"
#include "masklayer.h"
#include <QPainterPath>
#include "Paint/Widgets/Layers/picture_layer.h"

class ClipLayer : public QGraphicsObject
{
    Q_OBJECT
public:
    ClipLayer(QGraphicsItem* parent=nullptr);
    QRectF boundingRect()const override;
    QPainterPath shape()const override;
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent* event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = nullptr) override;
    void prepareSave();
    void endSave();
    void capture(QPixmap pix);
    void setPic(QPixmap pix);
    QRectF getClipRect();
    void reset();
    void sendRequestImage();
signals:
    void requestImage();
private:
    QPolygonF polygon;
    QPointF begin_point;
    QPointF end_point;
    QRect screen_rect;
    bool is_drag;
    bool begin_clip;
    bool is_save;
    Qt::MouseButton button;
    int drag_index;
    QList<ClipRegion*> regions;
    QPixmap pix;
    QPixmap savePix;
    MaskLayer* mask_layer;
};

#endif // CLIPREGION_H
