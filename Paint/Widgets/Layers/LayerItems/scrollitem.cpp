#include "scrollitem.h"
#include<QPainter>
#include<QDebug>
#include<QCursor>
#include<QGraphicsSceneMouseEvent>
#include<QVector2D>
#include<QVector3D>
#include<QtMath>

ScrollItem::ScrollItem(QGraphicsItem* parent) : QGraphicsObject(parent)
{
    pix = QPixmap(":/image/rotate.png");
    pix.scaled(16, 16);
    setCursor(QCursor(pix));
}

void ScrollItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    painter->drawPixmap(QPointF(-pix.width()/2.0, -pix.height()/2.0), pix);
}

QRectF ScrollItem::boundingRect() const
{
    return QRectF(QPointF(-pix.width()/2.0, -pix.height()/2.0), QSizeF(pix.width(), pix.height()));
}

void ScrollItem::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    begin_point = event->scenePos();
}

void ScrollItem::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    begin_point = mapToParent(mapFromScene(begin_point));
    QVector2D start_vec(begin_point.x(), begin_point.y());
    start_vec.normalize();
    QPointF point = mapToParent(mapFromScene(event->scenePos()));
    QVector2D end_vec(point.x(), point.y());
    end_vec.normalize();

    qreal dotValue = QVector2D::dotProduct(start_vec, end_vec);
    if (dotValue > 1.0)
        dotValue = 1.0;
    else if (dotValue < -1.0)
        dotValue = -1.0;

    dotValue = qAcos(dotValue);
    if (isnan(dotValue))
        dotValue = 0.0;

    qreal angle = dotValue * 1.0 / (M_PI / 180);

    QVector3D crossValue = QVector3D::crossProduct( \
            QVector3D(start_vec, 1.0), \
            QVector3D(end_vec, 1.0));

    if (crossValue.z() < 0)
        angle = -angle;
    emit angleChange(angle);
    begin_point = event->scenePos();
}
