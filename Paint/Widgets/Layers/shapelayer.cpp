#include "shapelayer.h"
#include<QGraphicsSceneMouseEvent>
#include "rect_layer.h"
#include "text_layer.h"
#include<QDebug>
#include "Helper/Reflect.h"
#include "Paint/Widgets/recorder.h"
#include "arrowlayer.h"
#include "string.h"
#include "Paint/Widgets/style_manager.h"
#include "Paint/Widgets/Recorder_element/shaperecord.h"
#include "Helper/math.h"

ShapeLayer::ShapeLayer(QGraphicsItem* parent) : QGraphicsObject(parent) {
    is_press = false;
    is_enable = false;
    is_focus = false;
    isRect = false;
    focus_item = NULL;
    shape = RECTANGLE;
    blur_layer = NULL;
//    blur_layer = new BlurLayer(this);
}

void ShapeLayer::reset() {
    focus_item = NULL;
    QList<QGraphicsItem*> children = childItems();
    for(QGraphicsItem* item: children) {
        delete item;
    }
    blur_layer = new BlurLayer(this);
}

void ShapeLayer::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) {
    if(isRect && is_press) {
        QPen pen;
        pen.setColor(QColor(161, 47, 47));
        pen.setWidth(3);
        pen.setJoinStyle(Qt::RoundJoin);
        painter->setPen(pen);
        painter->drawRect(Math::buildRect(begin_point, currentPoint));
    }
}

QRectF ShapeLayer::boundingRect() const {
    return QRectF();
}

void ShapeLayer::setPic(const QPixmap &pix, QPoint point) {
    this->pix = pix;
    this->point = point;
}

void ShapeLayer::mousePressEvent(QGraphicsSceneMouseEvent *event) {
    begin_point = mapFromScene(event->scenePos());
    if(is_enable && shape != BLUR) {
        if(!childContains(mapFromScene(begin_point)))
            is_press = true;
    }
    if(is_focus && focus_item != NULL) {
        QGraphicsItem* item = dynamic_cast<QGraphicsItem*>(focus_item);
        QPointF point = item->mapFromParent(mapFromScene(event->scenePos()));
        if(!item->contains(point)) {
            focus_item_func->loseFocusFunc();
            focus_item = NULL;
        }
    }
    if(is_enable && shape == DELETE_SHAPE) {
        is_press = true;
    }
    if(is_enable && shape == BLUR) {
        is_press = true;
        if(!blur_layer->isSetPic()) {
            blur_layer->setPix(pix, point);
        }
    }
    if(is_enable && shape == RECTANGLE) {
        isRect = true;
    }
}

void ShapeLayer::mouseMoveEvent(QGraphicsSceneMouseEvent *event) {
    currentPoint = mapFromScene(event->scenePos());
    if(is_enable && is_press && shape == BLUR) {
        blur_layer->addPoint(currentPoint.toPoint());
    }
    if(is_enable && is_press && shape == DELETE_SHAPE) {
        blur_layer->deletePoint(currentPoint.toPoint());
        deleteChildrens(point);
    }
    if(isRect) {
        update();
    }
}

void ShapeLayer::mouseReleaseEvent(QGraphicsSceneMouseEvent *event) {
    if(is_enable && is_press) {
        is_press = false;
        QPointF point = mapFromScene(event->scenePos());
        qreal left = 0, top=0, bottom=0, right=0;
        if(point.x() < begin_point.x()) {
            left = point.x();
            right = begin_point.x();
        } else {
            left = begin_point.x();
            right = point.x();
        }

        if(point.y() < begin_point.y()) {
            top = point.y();
            bottom = begin_point.y();
        } else {
            top = begin_point.y();
            bottom = point.y();
        }
        QRectF rect(QPointF(left, top), QPointF(right, bottom));
        switch(shape) {
        case RECTANGLE: {
            isRect = false;
            if(rect.width() < 10 || rect.height() < 10)
                break;
            RectLayer* rect_layer = new RectLayer(this, rect);
            rect_layer->setStyle(RectLayer::CUSTOM, Style_manager::instance()->get_now());
            rect_layer->setEnableMove(true);
            rect_layer->setEnableSizeChange(true);
            connect(rect_layer, &RectLayer::requestFocus, this, &ShapeLayer::onRequestFocus);
            setFocus(rect_layer, rect_layer);
            Recorder::instance()->record(new ShapeRecord(this, rect_layer, "undoRedoShapeFunc", false));
            break;
        }
        case TEXT: {
            if(childContains(begin_point)) {
                break;
            }
            Text_layer* text_layer = new Text_layer(rect, this);
            setFocus(text_layer, text_layer);
            connect(text_layer, &Text_layer::requestFocus, this, &ShapeLayer::onRequestFocus);
            break;
        }
        case PAINT_ARROW: {
            ArrowLayer* arrow_layer = new ArrowLayer(this, begin_point, point);
            setFocus(arrow_layer, arrow_layer);
            Recorder::instance()->record(new ShapeRecord(this, arrow_layer, "undoRedoShapeFunc", false));
            break;
        }
        case DELETE_SHAPE: {

        }
        }
    }
}

void ShapeLayer::setShape(SHAPE_TYPE shape) {
    this->shape = shape;
}

void ShapeLayer::setEnable(bool enable) {
    this->is_enable = enable;
    setLayersState();
}

void ShapeLayer::setGrabFocus(bool enable) {
    is_focus = enable;
    setLayersState();
}

void ShapeLayer::onRequestFocus(BaseLayer* object, QGraphicsObject* object2) {
    if(is_focus) {
        focus_item = object2;
        focus_item_func = object;
        object->getFocusFunc();
    }
//    if(shape == DELETE_SHAPE && is_enable)
//    {
//        Recorder::instance()->removeRecord(object2);
//        delete object2;
//        focus_item = NULL;
//        focus_item_func = NULL;
//    }
}

void ShapeLayer::deleteShape() {
//    if(focus_item != NULL)
//    {
//        Recorder::instance()->removeRecord(focus_item);
//        delete focus_item;
//        focus_item = NULL;
//        focus_item_func = NULL;
//    }
    shape = DELETE_SHAPE;
}

void ShapeLayer::setLayersState() {
    QList<QGraphicsItem*> items = childItems();
    const char* method_name;
    if(is_enable || is_focus) {
        method_name = "showNormal";
    } else {
        method_name = "hideNormal";

    }
    for(QGraphicsItem* item: items) {
        QGraphicsObject* object = qgraphicsitem_cast<QGraphicsObject*>(item);
        Reflect::invokeMethod(object->metaObject()->className(), object, method_name);
    }
    if(focus_item != NULL) {
        focus_item_func->loseFocusFunc();
    }

    focus_item = NULL;
    focus_item_func = NULL;
}

bool ShapeLayer::childContains(QPointF point) {
    QList<QGraphicsItem*> items = childItems();
    for(QGraphicsItem* item : items) {
        QPointF point1 = item->mapFromParent(point);
        QGraphicsObject* object = qgraphicsitem_cast<QGraphicsObject*>(item);
        bool ans;
        QGenericReturnArgument arg = Reflect::invokeMethod(object->metaObject()->className(), object, "acceptFocus", Q_RETURN_ARG(bool, ans));
        ans = *(bool*)arg.data();
        if(item->contains(point1) && item->isVisible() && ans) {
            return true;
        }
    }
    return false;
}

void ShapeLayer::deleteChildrens(QPointF point) {
    QList<QGraphicsItem*> items = childItems();
    for(QGraphicsItem* item : items) {
        QPointF point1 = item->mapFromParent(point);
        if(item->contains(point1) && item->isVisible()) {
            if(item == focus_item) {
                focus_item = NULL;
                focus_item_func = NULL;
            }
            QGraphicsObject* object = qgraphicsitem_cast<QGraphicsObject*>(item);
            if(!strcmp(object->metaObject()->className(), "BlurLayer")) {
                continue;
            }
            item->hide();
            Recorder::instance()->record(new ShapeRecord(this, object, "undoRedoShapeFunc", true));
//            Recorder::instance()->removeRecord(object);
//            delete item;
        }
    }
}

void ShapeLayer::prepareSave() {
    const char* method_name = "hideNormal";
    for(QGraphicsItem* item: childItems()) {
        QGraphicsObject* object = qgraphicsitem_cast<QGraphicsObject*>(item);

        Reflect::invokeMethod(object->metaObject()->className(), object, method_name);
    }
}

void ShapeLayer::endSave() {
    const char* method_name = "showNormal";
    for(QGraphicsItem* item: childItems()) {
        QGraphicsObject* object = qgraphicsitem_cast<QGraphicsObject*>(item);
        Reflect::invokeMethod(object->metaObject()->className(), object, method_name);
    }
}

void ShapeLayer::setFocus(QGraphicsObject *object, BaseLayer *func) {
    if(focus_item != NULL) {
        focus_item_func->loseFocusFunc();
    }

    focus_item = object;
    focus_item_func = func;
    func->getFocusFunc();
}

void ShapeLayer::changeBlur(bool is_range, int value) {
    if(is_range)
        blur_layer->setRange(value);
    else
        blur_layer->setUnitSize(value);
}

void ShapeLayer::undoRedoShapeFunc(bool show, QGraphicsObject *object) {
    if(show) {
        object->show();
    } else {
        object->hide();
    }
}

int ShapeLayer::type() const {
    return 65543;
}
