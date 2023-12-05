#include "blurlayercontainer.h"
#include <QSpinBox>
#include "../paintarea.h"

BlurLayerContainer::BlurLayerContainer(PaintArea* area) :
      LayerContainer(area),
      windowSize(10)
{
    blurLayer = new BlurLayer("blur", area, area->getRootLayer());
    blurLayer->setEnable(false);
    area->addLayer(blurLayer);
}

QWidget* BlurLayerContainer::onValid(QWidget *widgetParent) {
    area->setEnable(false);
    if(widget == NULL) {
        widget = new QWidget(widgetParent);
        QHBoxLayout* layout = new QHBoxLayout();
        QSpinBox* mosaicSize = new QSpinBox(widgetParent);
        mosaicSize->setRange(2, 30);
        mosaicSize->setValue(4);
        mosaicSize->setSingleStep(2);
        mosaicSize->setAccelerated(true);
        mosaicSize->setWrapping(true);
        mosaicSize->setKeyboardTracking(true);
        connect(mosaicSize, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), this, [=](int value) {
            windowSize =  value;
        });
        layout->addWidget(mosaicSize);
        widget->setLayout(layout);
    }
    return widget;
}

void BlurLayerContainer::layerMousePressEvent(QGraphicsSceneMouseEvent *event) {
    blurLayer->setParameter("windowSize", windowSize);
    blurLayer->addPoint(event->scenePos().toPoint());
}

void BlurLayerContainer::layerMouseMoveEvent(QGraphicsSceneMouseEvent *event) {
    blurLayer->addPoint(event->scenePos().toPoint());
}

void BlurLayerContainer::layerMouseReleaseEvent(QGraphicsSceneMouseEvent *event) {
    blurLayer->addPoint(event->scenePos().toPoint());
}
