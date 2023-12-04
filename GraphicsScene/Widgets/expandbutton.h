#ifndef EXPANDBUTTON_H
#define EXPANDBUTTON_H
#include <QGraphicsItem>
#include <QCursor>
#include <QGraphicsObject>
#include "PaintData.h"

class ExpandButton : public QGraphicsObject
{
    Q_OBJECT
public:
    enum ButtonDirection {NW, NE, SW, SE, N, W, S, E};
    ExpandButton();
    ExpandButton(QGraphicsItem* parent=nullptr);
    ExpandButton(ButtonDirection dir, QPointF pos, QGraphicsItem* parent=nullptr);
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;
    void hoverEnterEvent(QGraphicsSceneHoverEvent *event) override;
    void hoverLeaveEvent(QGraphicsSceneHoverEvent *event) override;
    void setLimit(QRectF rect);
    void setIndex(int index);
    void setButtonIndex(int index);
    void setPosition(QPointF pos);
    void setIntPos(QPoint pos);
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = nullptr) override;
    QRectF boundingRect() const override;
    QPainterPath shape() const override;
    bool setNeighbor(QPoint pos, int index);
    QPoint getPos();
    void clearIndex();
    QList<int> getIndex();
signals:
    void posChangeBegin(qreal x, qreal y);
    void posChange(ButtonDirection dir, qreal x, qreal y);
    void posChange(QList<int> index, qreal x, qreal y, int x_neighbor_index, int y_neighbor_index);
    void posTo(ButtonDirection dir, qreal x, qreal y);
private:
    ButtonDirection dir;
    QList<int> index;
    int buttonIndex;
    int x_neighbor_index;
    int y_neighbor_index;
    QCursor out_cursor;
    QPointF begin_point;
    QPointF record_point;
    QPoint int_point;
    QRectF limit;
    bool is_limit;
    bool is_capture;
    const qreal radius = 5;
};

#endif // EXPANDBUTTON_H
