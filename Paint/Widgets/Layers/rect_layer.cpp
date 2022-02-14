#include "rect_layer.h"
#include<QPen>
#include<QGraphicsSceneHoverEvent>
#include<QPainter>
#include<QDebug>

Rect_layer::Rect_layer(QGraphicsItem* parent, QRectF rect) : QGraphicsObject(parent)
{
    enable_move = false;
    enable_size_change = false;
    force_show = false;
    setAcceptHoverEvents(true);
    setPos(rect.topLeft());
    this->rect = QRectF(QPointF(0, 0), QSize(rect.width(), rect.height()));
    setBounding(this->rect);
    style = NORMAL;
}

void Rect_layer::hoverEnterEvent(QGraphicsSceneHoverEvent *event)
{
    out_cursor = cursor();
    if(enable_size_change)
    {
        setCursor(Qt::SizeAllCursor);
        showButtons();
    }
    QGraphicsItem::hoverEnterEvent(event);
}

void Rect_layer::hoverLeaveEvent(QGraphicsSceneHoverEvent *event)
{
    setCursor(out_cursor);
    hideButtons();
    QGraphicsItem::hoverLeaveEvent(event);
}

void Rect_layer::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    paintStyle(painter);
    painter->drawRect(this->boundingRect());
}

QPainterPath Rect_layer::shape() const
{
    QPainterPath ans;
    ans.addRect(rect);
    if(rect.width() > 10 && rect.height() > 10)
    {
        QRectF tmp = rect;
        tmp.setTopLeft(tmp.topLeft() + QPoint(5, 5));
        tmp.setBottomRight(tmp.bottomRight() - QPoint(5, 5));
        ans.addRect(tmp);
    }
    return ans;
}

void Rect_layer::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    begin_point = event->scenePos();
    emit requestFocus(this, this);
}

void Rect_layer::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    if(enable_move)
    {
        QPointF delta = event->scenePos() - begin_point;
        begin_point = event->scenePos();
        moveBy(delta.x(), delta.y());
        emit move(delta.x(), delta.y());
    }
}

void Rect_layer::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    if(enable_move)
    {
        QPointF delta = event->scenePos() - begin_point;
        moveBy(delta.x(), delta.y());
        emit move(delta.x(), delta.y());
    }
}

QRectF Rect_layer::boundingRect() const
{
    return this->rect;
}

void Rect_layer::showButtons()
{
    if(enable_size_change)
    {
        for(ExpandButton* button: buttons)
        {
            button->show();
            button->update();
        }
        update();
    }
}

void Rect_layer::hideButtons()
{
    if(force_show)
    {
        return;
    }
    for(ExpandButton* button: buttons)
    {
        button->hide();
    }
}

void Rect_layer::setBounding(QRectF rect)
{
    this->rect = rect;
    for(int i=0; i<4; i++)
    {
        direction dir = (direction)i;
        float x = dir == NW || dir == SW ? 0 : rect.width();
        float y = dir == NW || dir == NE ? 0 : rect.height();
        ExpandButton* button = new ExpandButton(dir, QPointF(x, y), this);
        connect(button, &ExpandButton::posChange, this, &Rect_layer::posChangeFunc);
        connect(button, &ExpandButton::posTo, this, &Rect_layer::posToFunc);
        buttons.insert(dir, button);
    }
    hideButtons();
}

void Rect_layer::posChangeFunc(direction dir, qreal x, qreal y)
{
    QRectF before_rect = rect;
    qreal width = buttons[NW]->boundingRect().width();
    before_rect.setTopLeft(before_rect.topLeft() - QPointF(width, width));
    before_rect.setBottomRight(before_rect.bottomRight() + QPointF(width, width));
    QPointF point(x, y);
    prepareGeometryChange();
    switch (dir)
    {
    case NW:
        rect.setTopLeft(rect.topLeft() + point);
        buttons[NE]->moveBy(0, y);
        buttons[SW]->moveBy(x, 0);
        break;
    case NE:
        rect.setTopRight(rect.topRight() + point);
        buttons[NW]->moveBy(0, y);
        buttons[SE]->moveBy(x, 0);
        break;
    case SW:
        rect.setBottomLeft(rect.bottomLeft() + point);
        buttons[SE]->moveBy(0, y);
        buttons[NW]->moveBy(x, 0);
        break;
    case SE:
        rect.setBottomRight(rect.bottomRight() + point);
        buttons[SW]->moveBy(0, y);
        buttons[NE]->moveBy(x, 0);
        break;
    default: qDebug() << "no implement this direction";break;
    }
    update(before_rect);
}

void Rect_layer::posToFunc(direction dir, qreal x, qreal y)
{
    emit sizeChange();
}

void Rect_layer::setEnableMove(bool enable)
{
    enable_move = enable;
}

void Rect_layer::setEnableSizeChange(bool enable)
{
    enable_size_change = enable;
}

void Rect_layer::getFocusFunc()
{
    showButtons();
    force_show = true;
}

void Rect_layer::loseFocusFunc()
{
    force_show = false;
    hideButtons();
}

void Rect_layer::setStyle(RECT_STYLE style)
{
    this->style = style;
}

void Rect_layer::paintStyle(QPainter *painter)
{
    switch(style)
    {
    case NORMAL:break;
    case RED:
        QPen pen;
        pen.setColor(QColor(161, 47, 47));
        pen.setWidth(3);
        pen.setJoinStyle(Qt::RoundJoin);
        painter->setPen(pen);
        break;
    }
}

void Rect_layer::hideNormal()
{
    enable_move = false;
    enable_size_change = false;
    force_show = false;
    hideButtons();
    update();
}

void Rect_layer::showNormal()
{
    enable_move = true;
    enable_size_change = true;
}
