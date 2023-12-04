#include "rectlayer.h"
#include <QDebug>

RectLayer::RectLayer(
    const QRectF& rect,
    const QString& name,
    ILayerControl* manager,
    QGraphicsItem* parent,
    bool enableResize) :
    LayerBase(name, manager, parent),
    scrollItem(NULL),
    enableScroll(false),
    buttonFocus(false)
{
    this->enableResize = enableResize;
    paintData.color = QColor(161, 47, 47);
    paintData.width = 3;
    paintData.joinStyle = Qt::RoundJoin;
    setAcceptHoverEvents(true);
    prepareGeometryChange();
    setPos(rect.topLeft());
    this->rect = QRectF(QPointF(0, 0), QSize(rect.width(), rect.height()));
    setBounding(this->rect);
}

void RectLayer::hoverEnterEvent(QGraphicsSceneHoverEvent *event) {
    if(!enable) return;
    if(enableResize) {
        outCursor = cursor();
        setCursor(Qt::SizeAllCursor);
        showButtons();
    }
}

void RectLayer::hoverLeaveEvent(QGraphicsSceneHoverEvent *event) {
    if(!enable) return;
    setCursor(outCursor);
    hideButtons();
}

void RectLayer::mousePressEvent(QGraphicsSceneMouseEvent *event) {
    beginPoint = event->scenePos();
    if(manager != NULL) manager->requestFocus(this);
}

void RectLayer::mouseMoveEvent(QGraphicsSceneMouseEvent *event) {
    if(enable) {
        QPointF delta = event->scenePos() - beginPoint;
        beginPoint = event->scenePos();
        moveBy(delta.x(), delta.y());
        emit move(delta.x(), delta.y());
    }
}

void RectLayer::mouseReleaseEvent(QGraphicsSceneMouseEvent *event) {
    if(enable) {
        QPointF delta = event->scenePos() - beginPoint;
        moveBy(delta.x(), delta.y());
        emit move(delta.x(), delta.y());
    }
}

void RectLayer::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) {
    painter->setRenderHints(QPainter::SmoothPixmapTransform | QPainter::Antialiasing, true);
    paintStyle(painter);
    painter->drawRect(this->boundingRect());
}

void RectLayer::setEnableResize(bool enable){
    enableResize = enable;
    if(enableResize && buttons.size() == 0){
        initButtons();
    }
}

void RectLayer::setBounding(const QRectF& rect) {
    this->rect = rect;
    if(buttons.size() == 0 && enableResize) {
        initButtons();
    } else {
        for(int i=0; i<buttons.size(); i++) {
            ExpandButton::ButtonDirection dir = (ExpandButton::ButtonDirection)i;
            QPointF p = rect.topLeft();
            float x = dir == ExpandButton::NW || dir == ExpandButton::SW ? p.x() : p.x() + rect.width();
            float y = dir == ExpandButton::NW || dir == ExpandButton::NE ? p.y() : p.y() + rect.height();
            buttons[dir]->setPos(x, y);
        }
    }
    hideButtons();
}

void RectLayer::posChangeFunc(ExpandButton::ButtonDirection dir, qreal x, qreal y) {
    QRectF before_rect = rect;
    qreal width = buttons[ExpandButton::NW]->boundingRect().width();
    before_rect.setTopLeft(before_rect.topLeft() - QPointF(width, width));
    before_rect.setBottomRight(before_rect.bottomRight() + QPointF(width, width));
    QPointF point(x, y);
    prepareGeometryChange();
    switch (dir) {
    case ExpandButton::NW:
        rect.setTopLeft(rect.topLeft() + point);
        buttons[ExpandButton::NE]->moveBy(0, y);
        buttons[ExpandButton::SW]->moveBy(x, 0);
        break;
    case ExpandButton::NE:
        rect.setTopRight(rect.topRight() + point);
        buttons[ExpandButton::NW]->moveBy(0, y);
        buttons[ExpandButton::SE]->moveBy(x, 0);
        break;
    case ExpandButton::SW:
        rect.setBottomLeft(rect.bottomLeft() + point);
        buttons[ExpandButton::SE]->moveBy(0, y);
        buttons[ExpandButton::NW]->moveBy(x, 0);
        break;
    case ExpandButton::SE:
        rect.setBottomRight(rect.bottomRight() + point);
        buttons[ExpandButton::SW]->moveBy(0, y);
        buttons[ExpandButton::NE]->moveBy(x, 0);
        break;
    default:
        qWarning() << "no implement this direction";
        break;
    }
    if(scrollItem != NULL)
        scrollItem->setPos(rect.left()+rect.width()/2, rect.top()-scrollItem->boundingRect().height());
    update(before_rect);
}

void RectLayer::posToFunc(ExpandButton::ButtonDirection dir, qreal x, qreal y) {
    emit sizeChange();
}

void RectLayer::showButtons() {
    if (enableResize) {
        for(ExpandButton* button: buttons) {
            button->show();
        }
    }
    if (enableScroll) {
        scrollItem->show();
    }
}

void RectLayer::hideButtons() {
    if (buttonFocus || (manager!= NULL && manager->hasFocus(this))) return;
    for (ExpandButton* button: buttons) {
        button->hide();
    }
    if(scrollItem != NULL) scrollItem->hide();
}

void RectLayer::setEnableScroll(bool enable){
    enableScroll = enable;
    if(enableScroll){
        scrollItem = new ScrollItem(this);
        scrollItem->setPos(QPointF(rect.width()/2, -scrollItem->boundingRect().height()));
        connect(scrollItem, &ScrollItem::angleChange, this, [=](qreal angle) {
            setTransform(transform().rotate(angle));
        });
    }
}

void RectLayer::paintStyle(QPainter *painter){
    QPen pen;
    pen.setColor(paintData.color);
    pen.setWidth(paintData.width);
    pen.setJoinStyle(paintData.joinStyle);
    pen.setCapStyle(paintData.capStyle);
    painter->setPen(pen);
}

void RectLayer::setLimit(const QRectF& limit) {
    for(ExpandButton* button: buttons) {
        button->setLimit(limit);
    }
}

void RectLayer::setStyle(const PaintData& data){
    paintData = data;
}
bool RectLayer::contains(const QPointF &point) const {
    if(QGraphicsObject::contains(point)) return true;
    for(QGraphicsItem* item: childItems()) {
        if(item->isVisible() && item->contains(item->mapFromParent(point)))return true;
    }
    return false;
}

void RectLayer::prepareSave(){
    LayerBase::prepareSave();
    showButtons();
}

void RectLayer::endSave(){
    LayerBase::endSave();
    hideButtons();
}

void RectLayer::initButtons(){
    for(int i=0; i<4; i++) {
        ExpandButton::ButtonDirection dir = (ExpandButton::ButtonDirection)i;
        QPointF p = rect.topLeft();
        float x = dir == ExpandButton::NW || dir == ExpandButton::SW ? p.x() : p.x() + rect.width();
        float y = dir == ExpandButton::NW || dir == ExpandButton::NE ? p.y() : p.y() + rect.height();
        ExpandButton* button = new ExpandButton(dir, QPointF(x, y), this);
        connect(button, static_cast<void (ExpandButton::*)(ExpandButton::ButtonDirection, qreal, qreal)>(&ExpandButton::posChange), this, &RectLayer::posChangeFunc);
        connect(button, &ExpandButton::posTo, this, &RectLayer::posToFunc);
        buttons.insert(dir, button);
    }
}

void RectLayer::setBoundEffect(){
    setEnableScroll(false);
    setEnableResize(true);
}

void RectLayer::reset(){
    if(manager != NULL) manager->removeThis(this);
}

void RectLayer::setParameter(const QString &name, const QVariant &var){
    if(name == "style"){
        paintData = var.value<PaintData>();
    }
}

QRectF RectLayer::boundingRect() const{
    return rect;
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

int RectLayer::getZValue() const{
    return 2;
}

void RectLayer::getFocus(){
    showButtons();
}

void RectLayer::loseFocus(){
    hideButtons();
}
