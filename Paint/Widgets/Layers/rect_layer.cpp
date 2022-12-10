#include "rect_layer.h"
#include<QPen>
#include<QGraphicsSceneHoverEvent>
#include<QPainter>
#include<QDebug>

Rect_layer::Rect_layer(QGraphicsItem* parent, QRectF rect)
    : QGraphicsObject(parent),
      scroll_item(new ScrollItem(this)),
      style(NORMAL),
      enable_move(false),
      enable_size_change(false),
      enable_scroll(true),
      force_show(false)
{
    setAcceptHoverEvents(true);
    setPos(rect.topLeft());
    this->rect = QRectF(QPointF(0, 0), QSize(rect.width(), rect.height()));
    setBounding(this->rect);
    scroll_item->setPos(QPointF(rect.width()/2, -scroll_item->boundingRect().height()));
    connect(scroll_item, &ScrollItem::angleChange, this, [=](qreal angle){
        setTransform(transform().rotate(angle));
    });
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
    painter->setRenderHints(QPainter::SmoothPixmapTransform | QPainter::Antialiasing, true);
    paintStyle(painter);
    painter->drawRect(this->boundingRect());
}

QPainterPath Rect_layer::shape() const
{
    QPainterPath ans;
    QRectF tmp1 = rect;
    tmp1.setTopLeft(tmp1.topLeft() - QPoint(5, 5));
    tmp1.setBottomRight(tmp1.bottomRight() + QPoint(5, 5));
    ans.addRect(tmp1);
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
        }
    }
    if(enable_scroll)
    {
        scroll_item->show();
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
    scroll_item->hide();
}

void Rect_layer::setBounding(QRectF rect)
{
    this->rect = rect;
    if(buttons.size() == 0)
    {
        for(int i=0; i<4; i++)
        {
            direction dir = (direction)i;
            QPointF p = rect.topLeft();
            float x = dir == NW || dir == SW ? p.x() : p.x() + rect.width();
            float y = dir == NW || dir == NE ? p.y() : p.y() + rect.height();
            ExpandButton* button = new ExpandButton(dir, QPointF(x, y), this);
            connect(button, static_cast<void (ExpandButton::*)(direction, qreal, qreal)>(&ExpandButton::posChange), this, &Rect_layer::posChangeFunc);
            connect(button, &ExpandButton::posTo, this, &Rect_layer::posToFunc);
            buttons.insert(dir, button);
        }
    }
    else
    {
        for(int i=0; i<buttons.size(); i++)
        {
            direction dir = (direction)i;
            QPointF p = rect.topLeft();
            float x = dir == NW || dir == SW ? p.x() : p.x() + rect.width();
            float y = dir == NW || dir == NE ? p.y() : p.y() + rect.height();
            buttons[dir]->setPos(x, y);
        }
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
    scroll_item->setPos(rect.left()+rect.width()/2, rect.top()-scroll_item->boundingRect().height());
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

void Rect_layer::setStyle(RECT_STYLE style, Paint_data data)
{
    this->style = style;
    this->data = data;
    update();
}

void Rect_layer::paintStyle(QPainter *painter)
{
    switch(style)
    {
    case NORMAL:break;
    case RED:
    {
        QPen pen;
        pen.setColor(QColor(161, 47, 47));
        pen.setWidth(3);
        pen.setJoinStyle(Qt::RoundJoin);
        painter->setPen(pen);
        break;
    }
    case CUSTOM:
    {
        QPen pen;
        pen.setColor(data.color);
        pen.setWidth(data.width);
        pen.setJoinStyle(data.join_style);
        pen.setCapStyle(data.cap_style);
        painter->setPen(pen);
        break;
    }
    }
}

void Rect_layer::hideNormal()
{
    enable_move = false;
    enable_size_change = false;
    force_show = false;
    enable_scroll = false;
    hideButtons();
    update();
}

void Rect_layer::showNormal()
{
    enable_move = true;
    enable_size_change = true;
    enable_scroll = true;
}

void Rect_layer::setLimit(QRectF limit)
{
    for(ExpandButton* button: buttons)
    {
        button->setLimit(limit);
    }
}

void Rect_layer::setEnableScroll(bool enable)
{
    enable_scroll = enable;
}

bool Rect_layer::contains(const QPointF &point) const
{
    if(QGraphicsObject::contains(point)) return true;
    for(QGraphicsItem* item: childItems())
    {
        if(item->isVisible() && item->contains(item->mapFromParent(point)))return true;
    }
    return false;
}

bool Rect_layer::acceptFocus()
{
    return true;
}

int Rect_layer::type() const
{
    return 65542;
}
