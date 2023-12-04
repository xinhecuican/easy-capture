#include "geolayercontainer.h"
#include <QButtonGroup>
#include "../../Helper/imagehelper.h"
#include "../../Style_widget/colorwidget.h"
#include <QSpinBox>
#include "../../Manager/uimanager.h"
#include "../../Helper/math.h"
#include "../Layer/rectlayer.h"
#include "../Layer/arrowlayer.h"
#include "../paintarea.h"
#include <QLineF>

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
        arrowButton->setToolTip("{D7HSBXWTLj}箭头");
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
}

void GeoLayerContainer::layerMouseMoveEvent(QGraphicsSceneMouseEvent *event){

}

void GeoLayerContainer::layerMouseReleaseEvent(QGraphicsSceneMouseEvent *event){
    QPointF endPoint = event->scenePos();
    switch (geoType) {
    case RECT: {
        QRectF rect = Math::buildRect(beginPoint, endPoint);
        if(rect.height() < 10 || rect.width() < 10) return;
        RectLayer* layer = new RectLayer(rect, "rect" + QString::number(rectId), area, NULL);
        rectId++;
        layer->setEnable(true);
        layer->setEnableScroll(true);
        area->addLayer(layer);
        break;
    }
    case ARROW: {
        QLineF line(beginPoint, endPoint);
        if(line.length() < 10) return;
        ArrowLayer* layer = new ArrowLayer(beginPoint, endPoint, "arrow" + QString::number(arrowId), area, NULL);
        arrowId++;
        layer->setEnable(true);
        area->addLayer(layer);
        break;
    }
    }
}
