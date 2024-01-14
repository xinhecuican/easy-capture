#include "masklayercontainer.h"
#include "../paintarea.h"
#include <QButtonGroup>

MaskLayerContainer::MaskLayerContainer(PaintArea* area) : LayerContainer(area)
{
    maskLayer = new MaskLayer("mask", area, area->getRootLayer());
    area->addLayer(maskLayer);
}

QWidget* MaskLayerContainer::onValid(QWidget *widgetParent) {
    area->setEnable(false, maskLayer->type());
    maskLayer->setEnable(true, maskLayer->type());
    if(!initWidget(widgetParent)){
        QButtonGroup* modeGroup = new QButtonGroup(widgetParent);
        modeGroup->setExclusive(true);

        QToolButton* rectCapture = new QToolButton(widgetParent);
        rectCapture->setIcon(ImageHelper::getIcon("rect"));
        rectCapture->setIconSize(QSize(DEFAULT_ICON_SIZE, DEFAULT_ICON_SIZE));
        rectCapture->setToolTip(MString::search("{OBwjJUhTkh}矩形窗口"));
        rectCapture->setChecked(true);
        rectCapture->setCheckable(true);
        QToolButton* freeCapture = new QToolButton(widgetParent);
        freeCapture->setIcon(ImageHelper::getIcon("painterpath"));
        freeCapture->setIconSize(QSize(DEFAULT_ICON_SIZE, DEFAULT_ICON_SIZE));
        freeCapture->setCheckable(true);
        modeGroup->addButton(rectCapture, 0);
        modeGroup->addButton(freeCapture, 1);
        connect(modeGroup, static_cast<void (QButtonGroup::*)(int)>(&QButtonGroup::buttonClicked)
                                , this, [=](int id) {
                    if(id == 0) {
                        Config::setConfig(Config::capture_mode, Config::RECT_CAPTURE);
                    } else if(id == 1) {
                        Config::setConfig(Config::capture_mode, Config::FREE_CAPTURE);
                    }
                });
        addWidget(rectCapture);
        addWidget(freeCapture);
    }
    return widget;
}

int MaskLayerContainer::regionCount(){
    return maskLayer->getRegionCount();
}
