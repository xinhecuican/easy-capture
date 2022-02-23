#include "arrowlayer.h"
#include<QPainter>
#include<QGraphicsSceneMouseEvent>
#include<QDebug>

ArrowLayer::ArrowLayer(QGraphicsItem* parent, QPointF begin_point, QPointF end_point)
    : QGraphicsObject(parent),
      is_focus(true),
      enable_move(true)
{
    setLine(begin_point, end_point);
    begin_button = new ExpandButton(W, begin_point, this);
    end_button = new ExpandButton(E, end_point, this);
    setAcceptHoverEvents(true);
    connect(begin_button, &ExpandButton::posChange, this, &ArrowLayer::posChangeFunc);
    connect(end_button, &ExpandButton::posChange, this, &ArrowLayer::posChangeFunc);
}

void ArrowLayer::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    painter->setRenderHint(QPainter::Antialiasing, true);                   //设置反走样，防锯齿
    QPen pen(Qt::red, 2, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
    QBrush brush(Qt::red, Qt::SolidPattern);
    painter->setPen(pen);
    painter->setBrush(brush);
    QLineF line(begin_point, end_point);
    painter->drawLine(line);
    painter->drawPolygon(arrow_points, 3);
}

QRectF ArrowLayer::boundingRect() const
{
    return QRectF(begin_point, end_point);
}

QPainterPath ArrowLayer::shape() const
{
    QLineF line(begin_point, end_point);
    line = line.unitVector();
    QLineF vertical_line = line;
    vertical_line.setAngle(line.angle() + 90);
    qreal half_width = ExtRefArrowLenght * sin(ExtRefArrowDegrees);
    QPainterPath path;
    QLineF new_line = vertical_line;
    new_line.setLength(half_width);
    path.moveTo(new_line.p2());
    QPointF delta = new_line.p2() - new_line.p1();
    path.lineTo(end_point + delta);
    path.lineTo(end_point - delta);
    path.lineTo(begin_point - delta);
    return path;
}

void ArrowLayer::hoverEnterEvent(QGraphicsSceneHoverEvent *event)
{
    out_cursor = cursor();
    if(enable_move)
    {
        setCursor(Qt::SizeAllCursor);
        showButtons();
    }
    QGraphicsObject::hoverEnterEvent(event);
}

void ArrowLayer::hoverLeaveEvent(QGraphicsSceneHoverEvent *event)
{
    setCursor(out_cursor);
    hideButtons();
    QGraphicsItem::hoverLeaveEvent(event);
}

void ArrowLayer::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    point = event->scenePos();
}

void ArrowLayer::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    if(enable_move)
    {
        QPointF delta_point = event->scenePos() - point;
        point = event->scenePos();
        moveBy(delta_point.x(), delta_point.y());
    }
}

void ArrowLayer::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    if(enable_move)
    {
        QPointF delta_point = event->scenePos() - point;
        point = event->scenePos();
        moveBy(delta_point.x(), delta_point.y());
    }
}

void ArrowLayer::setLine(QPointF begin_point, QPointF end_point)
{
    this->begin_point = begin_point;
    this->end_point = end_point;
    createArrow();
}

void ArrowLayer::createArrow()
{
    //箭头直线与水平方向的夹角再加pi
    float angle = atan2(end_point.y()-begin_point.y(), end_point.x()-begin_point.x()) + 3.1415926;
    //这两个值需要根据实际场景的坐标大小进行调整，


    arrow_points[0] = end_point;
    //求得箭头点1坐标
    arrow_points[1].setX(end_point.x() + ExtRefArrowLenght * cos(angle - ExtRefArrowDegrees));
    arrow_points[1].setY(end_point.y() + ExtRefArrowLenght * sin(angle - ExtRefArrowDegrees));
    //求得箭头点2坐标
    arrow_points[2].setX(end_point.x() + ExtRefArrowLenght * cos(angle + ExtRefArrowDegrees));
    arrow_points[2].setY(end_point.y() + ExtRefArrowLenght * sin(angle + ExtRefArrowDegrees));
}

void ArrowLayer::posChangeFunc(direction dir, qreal x, qreal y)
{
    if(dir == W)
    {
        begin_point = begin_point + QPointF(x, y);
    }
    else
    {
        end_point = end_point + QPointF(x, y);
        createArrow();
    }
    QRectF rect = boundingRect();
    rect.setTopLeft(rect.topLeft()-QPointF(5, 5));
    rect.setBottomRight(rect.bottomRight()+QPointF(5, 5));
    update(rect);
}

void ArrowLayer::getFocusFunc()
{
    showButtons();
    is_focus = true;
}

void ArrowLayer::loseFocusFunc()
{
    is_focus = false;
    hideButtons();
}

void ArrowLayer::showNormal()
{
    enable_move = true;
}

void ArrowLayer::hideNormal()
{
    loseFocusFunc();
    enable_move = false;
}

void ArrowLayer::showButtons()
{
    begin_button->show();
    end_button->show();
}

void ArrowLayer::hideButtons()
{
    if(is_focus)
    {
        return;
    }
    begin_button->hide();
    end_button->hide();
}
