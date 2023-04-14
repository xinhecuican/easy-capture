#include "paintlayer.h"
#include<QDir>
#include<QPainter>
#include<QDebug>
#include<QWidget>
#include "Paint/Widgets/recorder.h"
#include "Paint/Widgets/style_manager.h"
#include<QGraphicsSceneMouseEvent>
#include "Paint/Widgets/recorder.h"
#include "Paint/Widgets/Recorder_element/paintdeleterecord.h"

PaintLayer:: PaintLayer(QGraphicsItem* parent) : QGraphicsObject(parent) {
    this->parent = parent;
    is_enable = true;
    is_press = false;
    is_erase = false;
    _isPixSet = false;
    setAcceptedMouseButtons(Qt::NoButton);
}

PaintLayer::~PaintLayer() {
    for(QGraphicsItem* line : lines) {
        delete line;
    }
    if(_isPixSet) {
        delete pixPainter;
    }
}

void  PaintLayer::reset() {
    for(QGraphicsItem* line : lines) {
        delete line;
    }
    lines.clear();
    if(_isPixSet) {
        delete pixPainter;
        _isPixSet = false;
    }
}

void  PaintLayer::mousePressEvent(QGraphicsSceneMouseEvent *event) {
    updateAnchor = 0;
    if(event->button() == Qt::LeftButton) {
        is_press = true;
    }
    if(is_enable) {
        PaintData* paint_data = Style_manager::instance()->get();
        PaintInfo info;
        info.style_info = paint_data;
        info.path.append(mapFromScene(event->scenePos()));
        current_item = new PaintItem(info, this); // 添加一个线条
    }
    if(is_erase) {
        removeLines(mapFromScene(event->scenePos()));
    }
}

void  PaintLayer::mouseMoveEvent(QGraphicsSceneMouseEvent *event) {
    if(is_enable && is_press) {
        current_item->addPoint(mapFromScene(event->scenePos()));
    }
    if(is_erase && is_press) {
        removeLines(mapFromScene(event->scenePos()));
    }
}

void  PaintLayer::mouseReleaseEvent(QGraphicsSceneMouseEvent *event) {
    if(is_enable && is_press) {
        current_item->setEnd();
        current_item->addPoint(mapFromScene(event->scenePos()), true);
        current_item->paintLine(pixPainter);
        lines.append(current_item);
        PaintRecord* record = new PaintRecord(current_item);
        Recorder::instance()->record(record);
        update();
    }
    is_press = false;
}

void  PaintLayer::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) {
    if(_isPixSet)
        painter->drawPixmap(0, 0, pix);
}

void  PaintLayer::setEnableDraw(bool enable) {
    this->is_enable = enable;
}

QRectF  PaintLayer::boundingRect() const {
    return path.boundingRect();
}

void  PaintLayer::setErase(bool enable) {
    is_erase = enable;
}

void  PaintLayer::removeLines(QPointF point) {
    QRectF rect(point - QPointF(3, 3), point + QPointF(3, 3));
    QList<PaintItem*> items;
    for(PaintItem* item : lines) {
        if(item->shape().intersects(rect)) {
            items.append(item);
        }
    }
    for(PaintItem* item: items) {
        PaintDeleteRecord* record = new PaintDeleteRecord(this, item, "undoRedoPaintFunc");
        Recorder::instance()->record(record);
        lines.removeOne(item);
    }
    updateLines();
    update();
}

void  PaintLayer::undoRedoPaintFunc(bool is_undo, PaintItem *item) {
    if(is_undo) {
        item->show();
        lines.append(item);
    } else {
        item->hide();
        lines.removeOne(item);
    }
    updateLines();
}

int  PaintLayer::type() const {
    return 65539;
}

void PaintLayer::setPix(const QPixmap &pix, QPoint pos) {
    _isPixSet = true;
    this->pix = QPixmap(pix.width(), pix.height());
    this->pix.fill(Qt::transparent);
    setPos(pos);
    pixPainter = new QPainter(&this->pix);
    pixPainter->setRenderHints(QPainter::SmoothPixmapTransform | QPainter::Antialiasing, true);
}

void PaintLayer::updateLines() {
    if(!_isPixSet)
        return;
    pixPainter->fillRect(0, 0, pix.width(), pix.height(), QBrush(Qt::transparent));
    for(int i=0; i<lines.size(); i++) {
        lines.at(i)->paintLine(pixPainter);
    }
}
