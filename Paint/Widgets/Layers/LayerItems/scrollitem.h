#ifndef SCROLLITEM_H
#define SCROLLITEM_H
#include <QGraphicsObject>

class ScrollItem : public QGraphicsObject
{
    Q_OBJECT
public:
    ScrollItem(QGraphicsItem* parent);
    QRectF boundingRect()const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget=nullptr)override;
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;
signals:
    void angleChange(qreal angle);
private:
    QPixmap pix;
    QPointF begin_point;
};

#endif // SCROLLITEM_H
