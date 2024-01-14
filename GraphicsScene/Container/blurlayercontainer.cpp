#include "blurlayercontainer.h"
#include <QSpinBox>
#include "../paintarea.h"
#include "../../Helper/mstring.h"
#include "../../Helper/imagehelper.h"
#include <QButtonGroup>
#include <QDebug>

BlurLayerContainer::BlurLayerContainer(PaintArea* area) :
      LayerContainer(area),
      type(0),
      windowSize(10)
{
    blurLayer = new BlurLayer("blur", area, area->getRootLayer());
    blurLayer->setEnable(false);
    grabcutLayer = new GrabcutLayer("grabcut", area, area->getRootLayer());
    area->addLayer(blurLayer);
    area->addLayer(grabcutLayer);
}

QWidget* BlurLayerContainer::onValid(QWidget *widgetParent) {
    area->setEnable(false, blurLayer->type());
    if(type == 1){
        grabcutLayer->setEnableShow();
    }
    if(!initWidget(widgetParent)){
        QButtonGroup* group = new QButtonGroup(widgetParent);
        group->setExclusive(true);
        connect(group, static_cast<void (QButtonGroup::*)(int)>(&QButtonGroup::buttonClicked),
                this, [=](int id) {
                    type = id;
                    if(type == 1){
                        grabcutLayer->setEnableShow();
                    }
                });
        QToolButton* mosaicButton = new QToolButton(widgetParent);
        mosaicButton->setToolTip(MString::search("{capture_mosaic}马赛克"));
        mosaicButton->setIcon(ImageHelper::getIcon("blur"));
        mosaicButton->setIconSize(QSize(DEFAULT_ICON_SIZE, DEFAULT_ICON_SIZE));
        mosaicButton->setCheckable(true);
        mosaicButton->setChecked(true);
        group->addButton(mosaicButton, 0);
        QToolButton* grabcutButton = new QToolButton(widgetParent);
        grabcutButton->setToolTip(MString::search("{QbgMYc4u4H}透明"));
        grabcutButton->setIcon(ImageHelper::getIcon("mosaic"));
        grabcutButton->setIconSize(QSize(DEFAULT_ICON_SIZE, DEFAULT_ICON_SIZE));
        grabcutButton->setCheckable(true);
        group->addButton(grabcutButton, 1);

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
        addWidget(mosaicButton);
        addWidget(grabcutButton);
        addWidget(mosaicSize);
    }
    return widget;
}

void BlurLayerContainer::layerMousePressEvent(QGraphicsSceneMouseEvent *event) {
    if(type == 0){
        blurLayer->setParameter("windowSize", windowSize);
        blurLayer->addPoint(event->scenePos().toPoint());
    }
    else if(type == 1){
        background = event->button() == Qt::RightButton;
        grabcutLayer->addPoint(event->scenePos(), background, false);
    }
}

void BlurLayerContainer::layerMouseMoveEvent(QGraphicsSceneMouseEvent *event) {
    if(type == 0){
        blurLayer->addPoint(event->scenePos().toPoint());
    }
    else if(type == 1){
        grabcutLayer->addPoint(event->scenePos(), background, false);
    }
}

void BlurLayerContainer::layerMouseReleaseEvent(QGraphicsSceneMouseEvent *event) {
    if(type == 0){
        blurLayer->addPoint(event->scenePos().toPoint());
    }
    else if(type == 1){
        grabcutLayer->addPoint(event->scenePos(), background, true);
    }
}
