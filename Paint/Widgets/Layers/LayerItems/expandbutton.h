#ifndef EXPANDBUTTON_H
#define EXPANDBUTTON_H
#include <QGraphicsItem>
#include "Paint/Data/Common.h"
#include <QCursor>
#include <QGraphicsObject>

class ExpandButton : public QGraphicsObject
{
    Q_OBJECT
public:
    ExpandButton(direction dir, QPointF pos, QGraphicsItem* parent=nullptr);
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;
    void hoverEnterEvent(QGraphicsSceneHoverEvent *event) override;
    void hoverLeaveEvent(QGraphicsSceneHoverEvent *event) override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = nullptr) override;
    QRectF boundingRect() const override;
signals:
    void posChange(direction dir, qreal x, qreal y);
    void posTo(direction dir, qreal x, qreal y);
private:
    direction dir;
    QCursor out_cursor;
    QPointF begin_point;
    QPointF record_point;
    const qreal radius = 5;
};

#endif // EXPANDBUTTON_H
