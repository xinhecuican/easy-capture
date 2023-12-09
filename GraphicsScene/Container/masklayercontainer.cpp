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
    if(widget == NULL){
        widget = new QWidget(widgetParent);
        QHBoxLayout* layout = new QHBoxLayout();

        QButtonGroup* modeGroup = new QButtonGroup(widgetParent);
        modeGroup->setExclusive(true);

        QToolButton* rectCapture = new QToolButton(widgetParent);
        rectCapture->setIcon(ImageHelper::getIcon("rect"));
        rectCapture->setToolTip(MString::search("{OBwjJUhTkh}矩形窗口"));
        rectCapture->setChecked(true);
        rectCapture->setCheckable(true);
        rectCapture->setIconSize(QSize(36, 36));
        QToolButton* freeCapture = new QToolButton(widgetParent);
        freeCapture->setIcon(ImageHelper::getIcon("painterpath"));
        freeCapture->setIconSize(QSize(36, 36));
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
        layout->addWidget(rectCapture);
        layout->addWidget(freeCapture);
        widget->setLayout(layout);
    }
    return widget;
}
