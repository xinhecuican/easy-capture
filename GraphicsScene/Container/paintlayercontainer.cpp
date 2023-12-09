#include "paintlayercontainer.h"
#include <QButtonGroup>
#include "../../Helper/imagehelper.h"
#include "../../Style_widget/colorwidget.h"
#include <QSpinBox>
#include "../../Manager/uimanager.h"
#include "../../Helper/math.h"
#include "../paintarea.h"
#include <QLineF>

PaintLayerContainer::PaintLayerContainer(PaintArea* area) :
      LayerContainer(area),
      type(0),
      paintId(0)
{
    pencilData = UIManager::instance()->getPencilData();
    highlighterData = UIManager::instance()->getHighlighterData();
}

QWidget* PaintLayerContainer::onValid(QWidget *widgetParent) {
    area->setEnable(false, 102405);
    if(widget == NULL){
        widget = new QWidget(widgetParent);
        QHBoxLayout* layout = new QHBoxLayout();
        QButtonGroup* shapeGroup = new QButtonGroup(widgetParent);
        shapeGroup->setExclusive(true);
        connect(shapeGroup, static_cast<void (QButtonGroup::*)(int)>(&QButtonGroup::buttonClicked),
                this, [=](int id) {
                    switch(id) {
                    case 0: type = Pencil; break;
                    case 1: type = Highlighter; break;
                    default:
                        type = id;
                        qWarning() << "未知形状";
                        break;
                    }
                });
        QToolButton* pencilButton = new QToolButton(widgetParent);
        pencilButton->setObjectName("pencilButton");
        pencilButton->setToolTip("笔");
        pencilButton->setIcon(ImageHelper::getIcon("pencil"));
        pencilButton->setCheckable(true);
        pencilButton->setChecked(true);
        shapeGroup->addButton(pencilButton, 0);
        QToolButton* highlighterButton = new QToolButton(widgetParent);
        highlighterButton->setObjectName("highlighterButton");
        highlighterButton->setIcon(ImageHelper::getIcon("highlighter"));
        highlighterButton->setToolTip(MString::search("{j54u1kWtCx}荧光笔"));
        highlighterButton->setCheckable(true);
        shapeGroup->addButton(highlighterButton, 1);
        ColorWidget* colorWidget = new ColorWidget(widgetParent);
        connect(colorWidget, &ColorWidget::colorChange, this, [=](const QColor& color){
            switch(type){
            case Pencil: pencilData.color = color; break;
            case Highlighter: highlighterData.color = color; break;
            }
        });
        QSpinBox* widthButton = new QSpinBox(widgetParent);
        widthButton->setRange(1, 50);
        widthButton->setValue(3);
        widthButton->setAccelerated(true);
        widthButton->setWrapping(true);
        widthButton->setKeyboardTracking(true);
        connect(widthButton, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), this, [=](int value) {
            switch(type){
            case Pencil: pencilData.width = value; break;
            case Highlighter: highlighterData.width = value; break;
            }
        });

        layout->addWidget(pencilButton);
        layout->addWidget(highlighterButton);
        layout->addWidget(colorWidget);
        layout->addWidget(widthButton);
        widget->setLayout(layout);
    }
    return widget;
}

void PaintLayerContainer::layerMousePressEvent(QGraphicsSceneMouseEvent *event) {
    paintLayer = new PaintLayer("line" + QString::number(paintId), area, area->getRootLayer());
    switch(type) {
    case Pencil:
        paintLayer->setParameter("color", pencilData.color);
        paintLayer->setParameter("width", pencilData.width);
        break;
    case Highlighter:
        paintLayer->setParameter("color", highlighterData.color);
        paintLayer->setParameter("width", highlighterData.width);
        break;
    }
    paintLayer->setEnable(false);
    paintLayer->addPoint(event->scenePos());
    beforeTime = QDateTime::currentMSecsSinceEpoch();
}

void PaintLayerContainer::layerMouseMoveEvent(QGraphicsSceneMouseEvent *event) {
    qint64 currentTime = QDateTime::currentMSecsSinceEpoch();
    if(currentTime - beforeTime > 30) {
        beforeTime = currentTime;
        paintLayer->addPoint(event->scenePos());
    }
}

void PaintLayerContainer::layerMouseReleaseEvent(QGraphicsSceneMouseEvent *event) {
    paintLayer->addPoint(event->scenePos());
    paintId++;
    area->addLayer(paintLayer);
}
