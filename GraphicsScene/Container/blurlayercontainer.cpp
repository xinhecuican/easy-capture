#include "blurlayercontainer.h"
#include <QSpinBox>
#include "../paintarea.h"
#include "../../Helper/mstring.h"
#include "../../Helper/imagehelper.h"
#include <QButtonGroup>
#include <QDebug>
#include <QStandardItemModel>

BlurLayerContainer::BlurLayerContainer(PaintArea* area) :
      LayerContainer(area),
      type(0),
      windowSize(10),
      valid(false)
{
    blurLayer = new BlurLayer("blur", area, area->getRootLayer());
    blurLayer->setEnable(false);
    grabcutLayer = new GrabcutLayer("grabcut", area, area->getRootLayer());
    maskColorLayer = new MaskColorLayer("maskColor", area, area->getRootLayer());
    colorPicker = new ColorPicker(area, maskColorLayer);
    colorPicker->hide();
    area->addLayer(blurLayer);
    area->addLayer(grabcutLayer);
    area->addLayer(maskColorLayer);
    area->registerMouseMoveHook([=](QPointF pos){
        if(type == 2 && valid){
            colorPicker->setPosition(pos);
        }
    });
    area->registerMousePressHook([=](QPointF pos){
        if(type == 2 && valid){
            addColor(area->getImage().pixel(pos.x(), pos.y()));
        }
    });
    maskColorCombo = NULL;
}

QWidget* BlurLayerContainer::onValid(QWidget *widgetParent) {
    valid = true;
    area->setEnable(false, blurLayer->type());
    if(type == 1){
        grabcutLayer->setEnableShow();
        grabcutLayer->setFocus();
    }
    else if(type == 2){
        colorPicker->show();
    }
    if(!initWidget(widgetParent)){
        QButtonGroup* group = new QButtonGroup(widgetParent);
        group->setExclusive(true);
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
        QToolButton* colorMaskButton = new QToolButton(widgetParent);
        colorMaskButton->setIcon(ImageHelper::getIcon("color_mask"));
        colorMaskButton->setIconSize(QSize(DEFAULT_ICON_SIZE, DEFAULT_ICON_SIZE));
        colorMaskButton->setCheckable(true);
        group->addButton(colorMaskButton, 2);

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

        helpButton = new QToolButton(widgetParent);
        helpButton->setIcon(ImageHelper::getIcon("help"));
        helpButton->setToolTip(MString::search("{9VxwER0qFT}左键选择保留区域") +
                               "\n" +
                               MString::search("{6IqemrYceD}右键选择要删除的颜色"));
        helpButton->hide();

        maskColorCombo = new QComboBox(widgetParent);
        maskColorCombo->setEditable(false);
        maskColorCombo->setAutoFillBackground(true);
        connect(maskColorCombo, static_cast<void (QComboBox::*)(const QString &text)>(&QComboBox::currentIndexChanged),
                this, [=](const QString &text){
                    int splitter1 = text.indexOf(',');
                    int splitter2 = text.indexOf(',', splitter1+1);
                    int red = text.mid(0, splitter1).toInt();
                    int green = text.mid(splitter1+1, splitter2-splitter1-1).toInt();
                    int blue = text.mid(splitter2+1).toInt();
                    maskColorCombo->setStyleSheet(QString("background-color: rgb(%1,%2,%3)").arg(red).arg(green).arg(blue));
                });
        maskColorCombo->hide();
        removeButton = new QToolButton(widgetParent);
        removeButton->setIcon(ImageHelper::getIcon("delete"));
        connect(removeButton, &QToolButton::clicked, this, [=](){
            if(maskColorCombo->count()<=0){
                return;
            }
            int index = maskColorCombo->currentIndex();
            maskColorCombo->removeItem(index);
            maskColorLayer->removeMaskColor(index);
        });
        removeButton->hide();

        addWidget(mosaicButton);
        addWidget(grabcutButton);
        addWidget(colorMaskButton);
        addWidget(mosaicSize);
        addWidget(helpButton);
        addWidget(maskColorCombo);
        addWidget(removeButton);
        connect(group, static_cast<void (QButtonGroup::*)(int)>(&QButtonGroup::buttonClicked),
                this, [=](int id) {
                    type = id;
                    mosaicSize->hide();
                    colorPicker->hide();
                    helpButton->hide();
                    maskColorCombo->hide();
                    removeButton->hide();
                    if(type == 0){
                        mosaicSize->show();
                    }
                    else if(type == 1){
                        grabcutLayer->setFocus();
                        grabcutLayer->setEnableShow();
                        helpButton->show();
                    }
                    else{
                        maskColorCombo->show();
                        removeButton->show();
                        colorPicker->show();
                    }
                    widgetParent->adjustSize();
                });
    }
    return widget;
}

void BlurLayerContainer::onInvalid(){
    valid = false;
    colorPicker->hide();
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

int BlurLayerContainer::getCurrentType(){
    return type;
}

void BlurLayerContainer::addColor(QColor color){
    maskColorCombo->addItem(QString::number(color.red())+","+QString::number(color.green())
                   +","+QString::number(color.blue()));
    QStandardItemModel* model = qobject_cast<QStandardItemModel*>(maskColorCombo->model());
    model->item(maskColorCombo->count()-1)->setBackground(color);
    int red, green, blue;
    color.getRgb(&red, &green, &blue);
    red = red + (red - 127) * 0.5;
    red = (red < 0x00) ? 0xff+red : (red > 0xff) ? red-0xff : red;
    green = green + (green - 127) *  0.5;
    green = (green < 0x00) ? 0xff+green : (green > 0xff) ? green - 0xff : green;
    blue = blue + (blue - 127) * 0.5;
    blue = (blue < 0x00) ? 0xff+blue : (blue > 0xff) ? blue - 0xff : blue;
    model->item(maskColorCombo->count()-1)->setForeground(QColor(red, green, blue));
    maskColorCombo->setCurrentIndex(maskColorCombo->count()-1);
    maskColorLayer->addMaskColor(color);
    widget->parentWidget()->adjustSize();
}

void BlurLayerContainer::reset(){
    if(maskColorCombo != NULL){
        colorPicker->hide();
        helpButton->hide();
        maskColorCombo->hide();
        removeButton->hide();
        maskColorCombo->clear();
        type = 0;
    }
}
