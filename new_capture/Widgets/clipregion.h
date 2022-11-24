#ifndef CLIPREGION_H
#define CLIPREGION_H
#include <QGraphicsObject>
#include "Paint/Widgets/Layers/LayerItems/expandbutton.h"
#include "Paint/Widgets/Layers/baselayer.h"

class ClipRegion : public QGraphicsObject, public BaseLayer
{
    Q_OBJECT
public:
    ClipRegion(QPolygonF polygon, QGraphicsItem* parent=nullptr);
    ClipRegion(QPolygonF polygon, bool isButtonHide, QGraphicsItem* parent=nullptr);
    ~ClipRegion();
    bool intersect(QRectF rect);
    bool intersect(QPolygonF polygon);
    void addRect(QRectF rect);
    void addPolygon(QPolygonF polygon);
    void united(QPolygonF polygon);
    void united(ClipRegion* region);
    bool contain(QPointF point);
    void move(qreal dx, qreal dy);
    void mousePressEvent(QGraphicsSceneMouseEvent* event)override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent* event)override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = nullptr) override;
    void hideButton();
    QPolygonF getPolygon();
    QPainterPath shape()const override;
    QRectF boundingRect() const override;
    Q_INVOKABLE void hideNormal() override;
    Q_INVOKABLE void showNormal() override;
signals:
    void regionChange();
private:
    void remapPoint();
    bool isButtonHide;
    QPolygonF polygon;
    QList<ExpandButton*> buttons;
    QPointF begin_point;
};

#endif // CLIPREGION_H
