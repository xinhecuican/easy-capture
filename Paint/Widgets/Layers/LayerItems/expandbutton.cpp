#include "expandbutton.h"
#include<QGraphicsSceneMouseEvent>
#include<QPainter>
#include<QDebug>

ExpandButton::ExpandButton()
{
    is_limit = false;
    is_capture = false;
    setAcceptHoverEvents(true);
}

ExpandButton::ExpandButton(QGraphicsItem* parent) : QGraphicsObject(parent)
{
    is_limit = false;
    is_capture = false;
    setAcceptHoverEvents(true);
}

ExpandButton::ExpandButton(direction dir, QPointF pos, QGraphicsItem* parent): QGraphicsObject(parent)
{
    this->dir = dir;
    setPos(pos);
    setAcceptHoverEvents(true);
    is_limit = false;
    is_capture = false;
}

void ExpandButton::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    begin_point = event->scenePos();
    record_point = event->scenePos();
    emit posChangeBegin(begin_point.x(), begin_point.y());
}

void ExpandButton::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    QPointF delta_point = event->scenePos() - begin_point;
    begin_point = event->scenePos();
    QPointF after_point = pos() + delta_point;
    if(is_limit && (after_point.x() > limit.right() || after_point.x() < limit.left()
            || after_point.y() > limit.bottom() || after_point.y() < limit.top()))
    {
        return;
    }
    moveBy(delta_point.x(), delta_point.y());
    emit posChange(dir, delta_point.x(), delta_point.y());
    emit posChange(index, delta_point.x(), delta_point.y(), x_neighbor_index, y_neighbor_index);
}

void ExpandButton::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    QPointF delta_point = event->scenePos() - begin_point;
    begin_point = event->scenePos();
    QPointF after_point = pos() + delta_point;
    if(is_limit && (after_point.x() > limit.right() || after_point.x() < limit.left()
            || after_point.y() > limit.bottom() || after_point.y() < limit.top()))
    {
        return;
    }
    moveBy(delta_point.x(), delta_point.y());
    delta_point = event->scenePos() - record_point;
    emit posTo(dir, delta_point.x(), delta_point.y());
}

void ExpandButton::hoverEnterEvent(QGraphicsSceneHoverEvent *event)
{
    out_cursor = cursor();
    if(is_capture)
    {
        setCursor(Qt::PointingHandCursor);
    }
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

QPainterPath ExpandButton::shape() const
{
    QPainterPath path;
    path.addEllipse(QRectF(-QPointF(radius, radius), QPointF(radius, radius)));
    return path;
}

void ExpandButton::setLimit(QRectF rect)
{
    is_limit = true;
    limit = rect;
}

void ExpandButton::setIndex(int index)
{
    this->index.append(index);
    is_capture = true;
}

void ExpandButton::setPosition(QPointF pos)
{
    setPos(pos);
}

bool ExpandButton::setNeighbor(QPoint pos, int index)
{
    if(this->pos().x() == pos.x())
    {
        x_neighbor_index = index;
        return true;
    }
    else if(this->pos().y() == pos.y())
    {
        y_neighbor_index = index;
        return true;
    }
    return false;
}

void ExpandButton::setIntPos(QPoint pos)
{
    this->int_point = pos;
}

QPoint ExpandButton::getPos()
{
    return int_point;
}

void ExpandButton::clearIndex()
{
    index.clear();
}

QList<int> ExpandButton::getIndex()
{
    return index;
}

void ExpandButton::setButtonIndex(int index)
{
    buttonIndex = index;
}
