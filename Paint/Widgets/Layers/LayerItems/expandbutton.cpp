#include "expandbutton.h"
#include<QGraphicsSceneMouseEvent>
#include<QPainter>
#include<QDebug>

ExpandButton::ExpandButton(direction dir, QPointF pos, QGraphicsItem* parent): QGraphicsObject(parent)
{
    this->dir = dir;
    setPos(pos);
    setAcceptHoverEvents(true);
}

void ExpandButton::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    begin_point = event->scenePos();
    record_point = event->scenePos();
}

void ExpandButton::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    QPointF delta_point = event->scenePos() - begin_point;
    begin_point = event->scenePos();
    moveBy(delta_point.x(), delta_point.y());
    emit posChange(dir, delta_point.x(), delta_point.y());
}

void ExpandButton::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    QPointF delta_point = event->scenePos() - begin_point;
    begin_point = event->scenePos();
    moveBy(delta_point.x(), delta_point.y());
    delta_point = event->scenePos() - record_point;
    emit posTo(dir, delta_point.x(), delta_point.y());
}

void ExpandButton::hoverEnterEvent(QGraphicsSceneHoverEvent *event)
{
    out_cursor = cursor();
    if(dir == N || dir == S)
    {
        setCursor(Qt::SizeVerCursor);
    }
    else if(dir == W || dir == E)
    {
        setCursor(Qt::SizeHorCursor);
    }
    else if(dir == NE || dir == SW)
    {
        setCursor(Qt::SizeBDiagCursor);
    }
    else
    {
        setCursor(Qt::SizeFDiagCursor);
    }
}

void ExpandButton::hoverLeaveEvent(QGraphicsSceneHoverEvent *event)
{
    setCursor(out_cursor);
}

void ExpandButton::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    QBrush brush(QColor(255, 255, 255, 128));
    painter->setBrush(brush);
    painter->drawEllipse(QPointF(0, 0), radius, radius);
}

QRectF ExpandButton::boundingRect() const
{
    return QRectF(-QPointF(radius, radius), QPointF(radius, radius));
}
