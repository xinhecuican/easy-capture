#include "geolayercontainer.h"
#include <QButtonGroup>
#include "../../Helper/imagehelper.h"
#include "../../Style_widget/colorwidget.h"
#include <QSpinBox>
#include "../../Manager/uimanager.h"
#include "../../Helper/math.h"
#include "../paintarea.h"
#include <QLineF>
#include "../Recorder/layerrecord.h"

GeoLayerContainer::GeoLayerContainer(PaintArea* area) :
    LayerContainer(area),
    geoType(RECT),
    rectId(0),
    arrowId(0)
{
    rectPaintData = UIManager::instance()->getRectData();
    arrowPaintData = UIManager::instance()->getArrowData();
}

QWidget* GeoLayerContainer::onValid(QWidget* widgetParent){
    area->setEnable(true);
    if(widget == NULL){
        widget = new QWidget(widgetParent);
        QHBoxLayout* layout = new QHBoxLayout();
        QButtonGroup* shapeGroup = new QButtonGroup(widgetParent);
        shapeGroup->setExclusive(true);
        connect(shapeGroup, static_cast<void (QButtonGroup::*)(int)>(&QButtonGroup::buttonClicked),
                this, [=](int id) {
                    switch(id) {
                    case 0: geoType = RECT; break;
                    case 1: geoType = ARROW; break;
                    default:
                        geoType = id;
                        qWarning() << "未知形状";
                        break;
                    }
        });
        QToolButton* rectButton = new QToolButton(widgetParent);
        rectButton->setObjectName("rectButton");
        rectButton->setToolTip("矩形");
        rectButton->setIcon(ImageHelper::getIcon("rect"));
        rectButton->setCheckable(true);
        rectButton->setChecked(true);
        shapeGroup->addButton(rectButton, 0);
        QToolButton* arrowButton = new QToolButton(widgetParent);
        arrowButton->setObjectName("arrowButton");
        arrowButton->setIcon(ImageHelper::getIcon("paint_arrow"));
        arrowButton->setToolTip(MString::search("{D7HSBXWTLj}箭头"));
        arrowButton->setCheckable(true);
        shapeGroup->addButton(arrowButton, 1);
        ColorWidget* colorWidget = new ColorWidget(widgetParent);
        connect(colorWidget, &ColorWidget::colorChange, this, [=](const QColor& color){
            switch(geoType){
            case RECT: rectPaintData.color = color; break;
            case ARROW: arrowPaintData.color = color; break;
            }
        });
        QSpinBox* widthButton = new QSpinBox(widgetParent);
        widthButton->setRange(1, 50);
        widthButton->setValue(3);
        widthButton->setAccelerated(true);
        widthButton->setWrapping(true);
        widthButton->setKeyboardTracking(true);
        connect(widthButton, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), this, [=](int value) {
            switch(geoType){
            case RECT: rectPaintData.width = value; break;
            case ARROW: arrowPaintData.width = value; break;
            }
        });

        layout->addWidget(rectButton);
        layout->addWidget(arrowButton);
        layout->addWidget(colorWidget);
        layout->addWidget(widthButton);
        widget->setLayout(layout);
    }
    return widget;
}

void GeoLayerContainer::layerMousePressEvent(QGraphicsSceneMouseEvent *event){
    beginPoint = event->scenePos();
    switch (geoType) {
    case RECT: {
        QRectF rect = Math::buildRect(beginPoint, beginPoint + QPointF(1, 1));
        rectLayer = new RectLayer(rect, "rect" + QString::number(rectId), area, area->getRootLayer(), false);
        rectLayer->setEnable(false);
        break;
    }
    case ARROW: {
        arrowLayer = new ArrowLayer(beginPoint, beginPoint + QPointF(1, 1), "arrow" + QString::number(arrowId), area, area->getRootLayer());
        arrowLayer->setZValue(arrowLayer->getZValue());
        arrowLayer->setEnable(false);
        break;
    }
    }
}

void GeoLayerContainer::layerMouseMoveEvent(QGraphicsSceneMouseEvent *event){
    switch (geoType) {
    case RECT: {
        QRectF rect = Math::buildRect(beginPoint, event->scenePos());
        rectLayer->setRect(rect);
        break;
    }
    case ARROW: {
        arrowLayer->setEndPoint(event->scenePos());
        break;
    }
    }
}

void GeoLayerContainer::layerMouseReleaseEvent(QGraphicsSceneMouseEvent *event){
    QPointF endPoint = event->scenePos();
    switch (geoType) {
    case RECT: {
        QRectF rect = Math::buildRect(beginPoint, endPoint);
        if(rect.height() < 10 || rect.width() < 10) {
            rectLayer->deleteLater();
            return;
        }
        LayerRecord* record = new LayerRecord(true, rectLayer, (rectLayer->type() << 4) + rectId);
        area->record(record);
        rectId++;
        rectLayer->setRect(rect);
        rectLayer->setEnable(true);
        rectLayer->setEnableResize(true);
        rectLayer->setEnableScroll(true);
        area->addLayer(rectLayer);
        break;
    }
    case ARROW: {
        QLineF line(beginPoint, endPoint);
        if(line.length() < 10) {
            arrowLayer->deleteLater();
            return;
        }
        LayerRecord* record = new LayerRecord(true, arrowLayer, (arrowLayer->type() << 4) + arrowId);
        area->record(record);
        arrowId++;
        arrowLayer->setLine(beginPoint, endPoint);
        arrowLayer->setEnable(true);
        area->addLayer(arrowLayer);
        break;
    }
    }
}
