#include "picitem.h"
#include <QPainter>
PicItem::PicItem(QGraphicsItem* parent) : QGraphicsItem(parent)
{

}

void PicItem::setPix(const QPixmap &pix){
    prepareGeometryChange();
    this->pix = pix;
    mask_pix = pix;
    this->bound.setHeight(pix.height());
    this->bound.setWidth(pix.width());
    mask = this->bound;
    update();
}

void PicItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget){
    painter->drawPixmap(mask.topLeft(), mask_pix, mask);
}

void PicItem::reset(){
    pix = QPixmap();
    mask_pix = QPixmap();
}

void PicItem::setMask(QRectF mask){
    this->mask = mask;
}

void PicItem::setDisableColor(int index, QColor color) {
    if(index != -1) {
        disable_color.removeAt(index);
    } else {
        disable_color.append(color);
    }
    QImage temp = pix.toImage();
    for(QColor color : disable_color) {
        QImage mask = temp.createMaskFromColor(color.rgb(), Qt::MaskOutColor);
        temp.setAlphaChannel(mask);
    }
    for(QColor color: save_disable_color) {
        QImage mask = temp.createMaskFromColor(color.rgb(), Qt::MaskOutColor);
        temp.setAlphaChannel(mask);
    }
    mask_pix = QPixmap::fromImage(temp);
    update();
}

void PicItem::setSaveDisableColor(int index, QColor color) {
    if(index != -1) {
        save_disable_color.removeAt(index);
    } else {
        save_disable_color.append(color);
    }
    QImage temp = pix.toImage();
    for(QColor color: save_disable_color) {
        QImage mask = temp.createMaskFromColor(color.rgb(), Qt::MaskOutColor);
        temp.setAlphaChannel(mask);
    }
    for(QColor color : disable_color) {
        QImage mask = temp.createMaskFromColor(color.rgb(), Qt::MaskOutColor);
        temp.setAlphaChannel(mask);
    }
    mask_pix = QPixmap::fromImage(temp);
    update();
}

bool PicItem::containsPicture() {
    return !pix.isNull();
}

QRectF PicItem::boundingRect() const{
    return mask;
}
