#include "picture_layer.h"
#include "Paint/Widgets/recorder.h"
#include<QBitmap>
#include<QGraphicsSceneHoverEvent>
#include "Paint/Widgets/Recorder_element/resize_record.h"
#include "Paint/Widgets/recorder.h"
#include "Helper/imagehelper.h"

Picture_layer::Picture_layer(QGraphicsItem* parent): QGraphicsObject(parent) {
    rect_layer = new RectLayer(this, boundingRect());
    picItem = new PicItem(this);
    connect(rect_layer, &RectLayer::sizeChange, this, [=]() {
        prepareGeometryChange();
        ResizeRecord* record = new ResizeRecord(this, "undoRedoSizeFunc",
                                                mask, rect_layer->boundingRect());
        Recorder::instance()->record(record);
        mask = rect_layer->boundingRect();
        picItem->setMask(mask);
        this->update();
    });
//    rect_layer->setAcceptHoverEvents(false);
    setAcceptHoverEvents(true);
    rect_layer->show();
    rect_layer->setEnableScroll(false);
    setCacheMode(CacheMode::ItemCoordinateCache);
    setFlag(QGraphicsItem::ItemIsSelectable);
}

void Picture_layer::setPixmap(const QPixmap &pix) {
    this->bound.setHeight(pix.height());
    this->bound.setWidth(pix.width());
    mask = this->bound;
    rect_layer->setBounding(this->boundingRect());
    rect_layer->setLimit(bound);
    picItem->setPix(pix);
}

QRectF Picture_layer::boundingRect() const {
    return this->mask;
}

void Picture_layer::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) {
//    painter->drawPixmap(mask.topLeft(), mask_pix, mask);
}

void  Picture_layer::reset() {
    picItem->reset();
    bound = QRectF();
    mask = QRectF();
}

void Picture_layer::setEnableMove(bool enable) {
    rect_layer->setEnableSizeChange(enable);
}

void Picture_layer::setDisableColor(int index, QColor color) {
    picItem->setDisableColor(index, color);
}

void Picture_layer::setSaveDisableColor(int index, QColor color) {
    picItem->setSaveDisableColor(index, color);
}

bool Picture_layer::containsPicture() {
    return picItem->containsPicture();
}

void Picture_layer::prepareSave() {
    rect_layer->hide();
}

void Picture_layer::endSave() {
    rect_layer->show();
}

void Picture_layer::undoRedoSizeFunc(bool is_undo, QRectF before_rect, QRectF after_rect) {
    prepareGeometryChange();
    if(is_undo) {
        mask = before_rect;
        rect_layer->setBounding(before_rect);
        picItem->setMask(mask);
    } else {
        mask = after_rect;
        rect_layer->setBounding(after_rect);
        picItem->setMask(mask);
    }
    update();
}

int Picture_layer::type() const {
    return 65541;
}
