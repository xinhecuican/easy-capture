#include "arrowlayer.h"
#include "../../Manager/uimanager.h"
#include "../Recorder/layerrecord.h"

ArrowLayer::ArrowLayer(const QPointF& beginPt, const QPointF& endPt, const QString& name, ILayerControl* manager, QGraphicsItem* parent) :
      LayerBase(name, manager, parent),
      press(false){
    beginButton = new ExpandButton(ExpandButton::W, beginPt, this);
    endButton = new ExpandButton(ExpandButton::E, endPt, this);
    setLine(beginPt, endPt);
    connect(beginButton, static_cast<void (ExpandButton::*)(ExpandButton::ButtonDirection, qreal, qreal)>(&ExpandButton::posChange), this, &ArrowLayer::posChangeFunc);
    connect(endButton, static_cast<void (ExpandButton::*)(ExpandButton::ButtonDirection, qreal, qreal)>(&ExpandButton::posChange), this, &ArrowLayer::posChangeFunc);
    setAcceptHoverEvents(true);
    data = UIManager::instance()->getArrowData();
    if(name != ""){
        index = name.mid(5).toInt();
    }
}

void ArrowLayer::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) {
    painter->save();
    painter->setRenderHint(QPainter::Antialiasing, true);                   //设置反走样，防锯齿
    QPen pen(data.color, 2, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
    QBrush brush(data.color, Qt::SolidPattern);
    painter->setPen(pen);
    painter->setBrush(brush);
    QLineF line(beginPt, endPt);
    painter->drawLine(line);
    painter->drawPolygon(arrow_points, 3);
    painter->restore();
}

QRectF ArrowLayer::boundingRect() const {
    QRectF ans(beginPt, endPt);
    ans.setTopLeft(ans.topLeft() - QPointF(3, 3));
    ans.setBottomRight(ans.bottomRight() + QPointF(3, 3));
    return ans;
}

QPainterPath ArrowLayer::shape() const {
    QLineF line(beginPt, endPt);
    line = line.unitVector();
    QLineF vertical_line = line;
    vertical_line.setAngle(line.angle() + 90);
    qreal half_width = ExtRefArrowLenght * sin(ExtRefArrowDegrees) + 5;
    QPainterPath path;
    QLineF new_line = vertical_line;
    new_line.setLength(half_width);
    path.moveTo(new_line.p2());
    QPointF delta = new_line.p2() - new_line.p1();
    path.lineTo(endPt + delta);
    path.lineTo(endPt - delta);
    path.lineTo(beginPt - delta);
    for(QGraphicsItem* item : childItems()) {
        path.addPath(item->shape());
    }
    path = path.simplified();
    return path;
}


void ArrowLayer::hoverEnterEvent(QGraphicsSceneHoverEvent *event) {
    outCursor = cursor();
    if(enable) {
        setCursor(Qt::SizeAllCursor);
        showButtons();
    }
    QGraphicsObject::hoverEnterEvent(event);
}

void ArrowLayer::hoverLeaveEvent(QGraphicsSceneHoverEvent *event) {
    setCursor(outCursor);
    hideButtons();
    QGraphicsItem::hoverLeaveEvent(event);
}

void ArrowLayer::mousePressEvent(QGraphicsSceneMouseEvent *event) {
    if(enable){
        point = event->scenePos();
        manager->requestFocus(this);
        press = true;
    }
}

void ArrowLayer::mouseMoveEvent(QGraphicsSceneMouseEvent *event) {
    if(enable && press) {
        QPointF delta_point = event->scenePos() - point;
        point = event->scenePos();
        moveBy(delta_point.x(), delta_point.y());
    }
}

void ArrowLayer::mouseReleaseEvent(QGraphicsSceneMouseEvent *event) {
    if(enable && press) {
        QPointF delta_point = event->scenePos() - point;
        point = event->scenePos();
        moveBy(delta_point.x(), delta_point.y());
    }
    press = false;
}

void ArrowLayer::setLine(const QPointF& beginPt, const QPointF& endPt) {
    this->beginPt = beginPt;
    this->endPt = endPt;
    beginButton->setPosition(beginPt);
    endButton->setPosition(endPt);
    createArrow();
}


void ArrowLayer::createArrow() {
    //箭头直线与水平方向的夹角再加pi
    float angle = atan2(endPt.y()-beginPt.y(), endPt.x()-beginPt.x()) + 3.1415926;
    //这两个值需要根据实际场景的坐标大小进行调整，


    arrow_points[0] = endPt;
    //求得箭头点1坐标
    arrow_points[1].setX(endPt.x() + ExtRefArrowLenght * cos(angle - ExtRefArrowDegrees));
    arrow_points[1].setY(endPt.y() + ExtRefArrowLenght * sin(angle - ExtRefArrowDegrees));
    //求得箭头点2坐标
    arrow_points[2].setX(endPt.x() + ExtRefArrowLenght * cos(angle + ExtRefArrowDegrees));
    arrow_points[2].setY(endPt.y() + ExtRefArrowLenght * sin(angle + ExtRefArrowDegrees));
}

void ArrowLayer::posChangeFunc(ExpandButton::ButtonDirection dir, qreal x, qreal y) {
    if(dir == ExpandButton::W) {
        beginPt = beginPt + QPointF(x, y);
        createArrow();
    } else {
        endPt = endPt + QPointF(x, y);
        createArrow();
    }
    QRectF rect = boundingRect();
    rect.setTopLeft(rect.topLeft()-QPointF(5, 5));
    rect.setBottomRight(rect.bottomRight()+QPointF(5, 5));
    update(rect);
}

void ArrowLayer::showButtons() {
    beginButton->show();
    endButton->show();
}

void ArrowLayer::hideButtons() {
    if(manager->hasFocus(this)) {
        return;
    }
    beginButton->hide();
    endButton->hide();
}

void ArrowLayer::getFocus(){
    showButtons();
}

void ArrowLayer::loseFocus(){
    hideButtons();
}

void ArrowLayer::setParameter(const QString &name, const QVariant &var){

}

void ArrowLayer::setStyle(const PaintData &data){
    this->data = data;
}
void ArrowLayer::reset(){
    manager->removeThis(this);
}

void ArrowLayer::setEndPoint(const QPointF &point) {
    this->endPt = point;
    setLine(beginPt, endPt);
}

int ArrowLayer::getZValue() const{
    return 2;
}

int ArrowLayer::type() const{
    return 102400;
}

void ArrowLayer::prepareSave(){
    LayerBase::prepareSave();
    beginButton->hide();
    endButton->hide();
}

bool ArrowLayer::contains(const QPointF &point) const{
    if(QGraphicsObject::contains(point)) return true;
    for(QGraphicsItem* item: childItems()) {
        if(item->isVisible() && item->contains(item->mapFromParent(point)))return true;
    }
    return false;
}

void ArrowLayer::onDelete(const QPointF &point){
    hide();
    LayerRecord* record = new LayerRecord(false, this, (type() << 4) + index);
    manager->record(record);
}
