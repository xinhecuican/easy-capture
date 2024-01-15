#ifndef CLIPREGION_H
#define CLIPREGION_H
#include <QGraphicsObject>
#include "expandbutton.h"

class ClipRegion : public QGraphicsObject
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
    void mouseReleaseEvent(QGraphicsSceneMouseEvent* event)override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = nullptr) override;
    void hideButton();
    void setEnable(bool enable);
    QPolygonF getPolygon();
    QPainterPath shape()const override;
    QRectF boundingRect() const override;
    void hideNormal();
    void showNormal();
signals:
    void regionPress();
    void regionChangeBegin(QPointF point);
    void regionChange();
    void regionMove(QPointF point);
private:
    void remapPoint();
    bool isButtonHide;
    bool is_enable;
    QPolygonF polygon;
    QList<ExpandButton*> buttons;
    QPointF begin_point;
};

#endif // CLIPREGION_H
