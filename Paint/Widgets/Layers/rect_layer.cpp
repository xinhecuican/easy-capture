#include "rect_layer.h"
#include<QPen>
#include<QGraphicsSceneHoverEvent>
#include<QPainter>
#include<QDebug>

RectLayer::RectLayer(QGraphicsItem* parent, QRectF rect)
    : QGraphicsObject(parent),
      scroll_item(new ScrollItem(this)),
      style(NORMAL),
      enable_move(false),
      enable_size_change(false),
      enable_scroll(true),
      force_show(false) {
    setAcceptHoverEvents(true);
    setPos(rect.topLeft());
    this->rect = QRectF(QPointF(0, 0), QSize(rect.width(), rect.height()));
    setBounding(this->rect);
    scroll_item->setPos(QPointF(rect.width()/2, -scroll_item->boundingRect().height()));
    connect(scroll_item, &ScrollItem::angleChange, this, [=](qreal angle) {
        setTransform(transform().rotate(angle));
    });
}

void RectLayer::hoverEnterEvent(QGraphicsSceneHoverEvent *event) {
    out_cursor = cursor();
    if(enable_size_change) {
        setCursor(Qt::SizeAllCursor);
        showButtons();
    }
    QGraphicsItem::hoverEnterEvent(event);
}

void RectLayer::hoverLeaveEvent(QGraphicsSceneHoverEvent *event) {
    setCursor(out_cursor);
    hideButtons();
    QGraphicsItem::hoverLeaveEvent(event);
}

void RectLayer::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) {
    painter->setRenderHints(QPainter::SmoothPixmapTransform | QPainter::Antialiasing, true);
    paintStyle(painter);
    painter->drawRect(this->boundingRect());
}

QPainterPath RectLayer::shape() const {
    QPainterPath ans;
    QRectF tmp1 = rect;
    tmp1.setTopLeft(tmp1.topLeft() - QPoint(5, 5));
    tmp1.setBottomRight(tmp1.bottomRight() + QPoint(5, 5));
    ans.addRect(tmp1);
    if(rect.width() > 10 && rect.height() > 10) {
        QRectF tmp = rect;
        tmp.setTopLeft(tmp.topLeft() + QPoint(5, 5));
        tmp.setBottomRight(tmp.bottomRight() - QPoint(5, 5));
        ans.addRect(tmp);
    }
    return ans;
}

void RectLayer::mousePressEvent(QGraphicsSceneMouseEvent *event) {
    begin_point = event->scenePos();
    emit requestFocus(this, this);
}

void RectLayer::mouseMoveEvent(QGraphicsSceneMouseEvent *event) {
    if(enable_move) {
        QPointF delta = event->scenePos() - begin_point;
        begin_point = event->scenePos();
        moveBy(delta.x(), delta.y());
        emit move(delta.x(), delta.y());
    }
}

void RectLayer::mouseReleaseEvent(QGraphicsSceneMouseEvent *event) {
    if(enable_move) {
        QPointF delta = event->scenePos() - begin_point;
        moveBy(delta.x(), delta.y());
        emit move(delta.x(), delta.y());
    }
}

QRectF RectLayer::boundingRect() const {
    return this->rect;
}

void RectLayer::showButtons() {
    if(enable_size_change) {
        for(ExpandButton* button: buttons) {
            button->show();
        }
    }
    if(enable_scroll) {
        scroll_item->show();
    }
}

void RectLayer::hideButtons() {
    if(force_show) {
        return;
    }
    for(ExpandButton* button: buttons) {
        button->hide();
    }
    scroll_item->hide();
}

void RectLayer::setBounding(QRectF rect) {
    this->rect = rect;
    if(buttons.size() == 0) {
        for(int i=0; i<4; i++) {
            direction dir = (direction)i;
            QPointF p = rect.topLeft();
            float x = dir == NW || dir == SW ? p.x() : p.x() + rect.width();
            float y = dir == NW || dir == NE ? p.y() : p.y() + rect.height();
            ExpandButton* button = new ExpandButton(dir, QPointF(x, y), this);
            connect(button, static_cast<void (ExpandButton::*)(direction, qreal, qreal)>(&ExpandButton::posChange), this, &RectLayer::posChangeFunc);
            connect(button, &ExpandButton::posTo, this, &RectLayer::posToFunc);
            buttons.insert(dir, button);
        }
    } else {
        for(int i=0; i<buttons.size(); i++) {
            direction dir = (direction)i;
            QPointF p = rect.topLeft();
            float x = dir == NW || dir == SW ? p.x() : p.x() + rect.width();
            float y = dir == NW || dir == NE ? p.y() : p.y() + rect.height();
            buttons[dir]->setPos(x, y);
        }
    }
    hideButtons();
}

void RectLayer::posChangeFunc(direction dir, qreal x, qreal y) {
    QRectF before_rect = rect;
    qreal width = buttons[NW]->boundingRect().width();
    before_rect.setTopLeft(before_rect.topLeft() - QPointF(width, width));
    before_rect.setBottomRight(before_rect.bottomRight() + QPointF(width, width));
    QPointF point(x, y);
    prepareGeometryChange();
    switch (dir) {
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
    default:
        qDebug() << "no implement this direction";
        break;
    }
    scroll_item->setPos(rect.left()+rect.width()/2, rect.top()-scroll_item->boundingRect().height());
    update(before_rect);
}

void RectLayer::posToFunc(direction dir, qreal x, qreal y) {
    emit sizeChange();
}

void RectLayer::setEnableMove(bool enable) {
    enable_move = enable;
}

void RectLayer::setEnableSizeChange(bool enable) {
    enable_size_change = enable;
}

void RectLayer::getFocusFunc() {
    showButtons();
    force_show = true;
}

void RectLayer::loseFocusFunc() {
    force_show = false;
    hideButtons();
}

void RectLayer::setStyle(RECT_STYLE style, PaintData data) {
    this->style = style;
    this->data = data;
    update();
}

void RectLayer::paintStyle(QPainter *painter) {
    switch(style) {
    case NORMAL:
        break;
    case RED: {
        QPen pen;
        pen.setColor(QColor(161, 47, 47));
        pen.setWidth(3);
        pen.setJoinStyle(Qt::RoundJoin);
        painter->setPen(pen);
        break;
    }
    case CUSTOM: {
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

void RectLayer::hideNormal() {
    enable_move = false;
    enable_size_change = false;
    force_show = false;
    enable_scroll = false;
    hideButtons();
    update();
}

void RectLayer::showNormal() {
    enable_move = true;
    enable_size_change = true;
    enable_scroll = true;
}

void RectLayer::setLimit(QRectF limit) {
    for(ExpandButton* button: buttons) {
        button->setLimit(limit);
    }
}

void RectLayer::setEnableScroll(bool enable) {
    enable_scroll = enable;
}

bool RectLayer::contains(const QPointF &point) const {
    if(QGraphicsObject::contains(point)) return true;
    for(QGraphicsItem* item: childItems()) {
        if(item->isVisible() && item->contains(item->mapFromParent(point)))return true;
    }
    return false;
}

bool RectLayer::acceptFocus() {
    return true;
}

int RectLayer::type() const {
    return 65542;
}

void RectLayer::setRect(QRectF rect) {
    this->rect = rect;
    update();
}
