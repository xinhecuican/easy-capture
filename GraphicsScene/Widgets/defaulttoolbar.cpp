#include "defaulttoolbar.h"
#include "../Container/blurlayercontainer.h"
#include "../Container/geolayercontainer.h"
#include "../Container/masklayercontainer.h"
#include "../Container/paintlayercontainer.h"
#include "../Container/textlayercontainer.h"
#include <QFileDialog>
#include "../../Paint/Widgets/history.h"

DefaultToolbar::DefaultToolbar(PaintArea* area, QWidget* parent) : LayerToolBar(area, parent)
{
    GeoLayerContainer* geoLayerContainer = new GeoLayerContainer(area);
    addContainer("shape", "", geoLayerContainer, true);
    BlurLayerContainer* blurLayerContainer = new BlurLayerContainer(area);
    addContainer("mosaic", "", blurLayerContainer, true);
    QToolButton* cursorButton = new QToolButton(this);
    cursorButton->setIcon(QIcon(":/image/cursor.png"));
    cursorButton->setToolTip(MString::search("{l4yTU9QXUd}指针"));
    cursorButton->setCursor(QCursor(QPixmap(":/image/cursor.png"), 0, 0));
    MaskLayerContainer* maskLayerContainer = new MaskLayerContainer(area);
    addContainer(cursorButton, maskLayerContainer, true);
    setContainer(maskLayerContainer);
    PaintLayerContainer* paintLayerContainer = new PaintLayerContainer(area);
    addContainer("pencil", "", paintLayerContainer, true);
    TextLayerContainer* textLayerContainer = new TextLayerContainer(area);
    addContainer("text", "", textLayerContainer, true);

    QToolButton* eraseButton = new QToolButton(this);
    eraseButton->setIcon(ImageHelper::getIcon("eraser"));
    eraseButton->setToolTip(MString::search("{7cwKObEhcx}擦除"));
    addContainer(eraseButton,
        [=](){area->setEnable(false);area->setEraseEnable(true);},
        [=](){area->setEraseEnable(false);}, true);

    addSeparator();

    QToolButton* undoButton = new QToolButton(this);
    undoButton->setIcon(ImageHelper::getIcon("undo"));
    undoButton->setToolTip(MString::search("{h5KymvIMTN}撤销"));
    connect(undoButton, &QToolButton::clicked, this, [=]() {
        area->getRecorder()->back();
    });
    addWidget(undoButton);

    QToolButton* redoButton = new QToolButton(this);
    redoButton->setToolTip(MString::search("{a7CaC7NOL5}恢复"));
    redoButton->setIcon(ImageHelper::getIcon("redo"));
    connect(redoButton, &QToolButton::clicked, this, [=]() {
        area->getRecorder()->forward();
    });
    addWidget(redoButton);
    connect(area, &PaintArea::recordChange, this, [=]() {
        if(!area->getRecorder()->undoAvailiable()) {
            undoButton->setEnabled(false);
        } else {
            undoButton->setEnabled(true);
        }
        if(!area->getRecorder()->redoAvailiable()) {
            redoButton->setEnabled(false);
        } else {
            redoButton->setEnabled(true);
        }
    });

    QToolButton* clipButton = new QToolButton(this);
    clipButton->setIcon(ImageHelper::getIcon("clipboard"));
    clipButton->setToolTip(MString::search("{ntbJbEqxwF}复制到剪切板"));
    connect(clipButton, &QToolButton::clicked, this, [=]() {
        if(area->save(ILayerControl::ClipBoard, ""))
            WindowManager::changeWindow("tray");
    });
    addWidget(clipButton);

    QToolButton* saveButton = new QToolButton(this);
    saveButton->setIcon(ImageHelper::getIcon("save"));
    saveButton->setToolTip(MString::search("{pJqTHhEQdb}保存"));
    connect(saveButton, &QToolButton::clicked, this, [=]() {
        QString fileName = QFileDialog::getSaveFileName(parent,
                                                        "保存",
                                                        History::instance()->get_last_directory(),
                                                        "图片(*.bmp *.jpg *.jpeg *.png);;所有文件(*)");
        if(fileName != "") {
            if(area->save(ILayerControl::Persist, fileName))
                WindowManager::changeWindow("tray");
        }
    });
    addWidget(saveButton);


    ClipLayerBase* clipLayer = area->getClipLayer();
    if(clipLayer != NULL){
        connect(clipLayer, &ClipLayerBase::boundChangeBegin, this, [=](){
            hideAll();
        });
        connect(clipLayer, &ClipLayerBase::boundChange, this, [=](QRectF bound){
            moveTo(bound);
            showAll();
        });
    }
    adjustSize();
    hideAll();
}
