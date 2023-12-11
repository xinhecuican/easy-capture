#include "textlayer.h"

TextLayer::TextLayer(const QPointF& pos,
                     const QString& name,
                     ILayerControl* manager,
                     QGraphicsItem* parent) :
    LayerBase(name, manager, parent),
    textItem(new TextItem(pos, this))
{
    connect(textItem, &TextItem::requestFocus, this, [=](){
        manager->requestFocus(this);
    });
    connect(textItem, &TextItem::formatChange, this, [=](const QFont& font, const QBrush& brush){
        emit formatChange(font, brush);
    });
}

void TextLayer::reset(){
    manager->removeThis(this);
}

void TextLayer::getFocus(){
    textItem->getFocus();
}

void TextLayer::loseFocus(){
    if(textItem->toPlainText() == ""){
        manager->removeThis(this);
        return;
    }
    textItem->loseFocus();
}

QRectF TextLayer::boundingRect() const{
    return QRectF();
}

void TextLayer::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget){

}

void TextLayer::prepareSave(){
    textItem->loseFocus();
}

void TextLayer::setParameter(const QString &name, const QVariant &var){
    if(name == "font"){
        QFont font = var.value<QFont>();
        textItem->setFont(font);
    }
    if(name == "color"){
        QColor color = var.value<QColor>();
        textItem->setColor(color);
    }
    if(name == "brush"){
        textItem->setBrush(true);
    }
}

void TextLayer::setEnable(bool enable, int index){
    LayerBase::setEnable(enable);
    textItem->setEnable(enable);
}

const QString TextLayer::getHelp(){
    return "font: 设置字体\n"
           "color: 设置字体颜色\n"
           "brush: 设置字体刷";
}

bool TextLayer::contains(const QPointF &point) const{
    if(QGraphicsObject::contains(point)) return true;
    for(QGraphicsItem* item: childItems()) {
        if(item->isVisible() && item->contains(item->mapFromParent(point)))return true;
    }
    return false;
}

int TextLayer::type() const{
    return 102408;
}

void TextLayer::onDelete(const QPointF &point){
    manager->removeThis(this);
}
