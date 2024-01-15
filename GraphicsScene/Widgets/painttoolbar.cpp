#include "painttoolbar.h"
#include "../Container/blurlayercontainer.h"
#include "../Container/geolayercontainer.h"
#include "../Container/textlayercontainer.h"
#include "../Layer/piclayer.h"
#include "../../Helper/imagehelper.h"
#include "../../Manager/WindowManager.h"
#include "../../Manager/history.h"
#include <QFileDialog>
#include "../../Manager/config.h"
#include "../../Helper/common.h"
#include <QMenu>

PaintToolbar::PaintToolbar(PaintArea* area, QWidget* parent) : LayerToolBar(area, parent)
{
    PicLayer* picLayer = new PicLayer("pic", area, true, NULL);
    PaintData data;
    data.width = 1;
    data.color = QColor(66, 165, 220);
    picLayer->setStyle(data);
    area->addLayer(picLayer);

    QToolButton* new_button = new QToolButton(this);
    connect(new_button, &QToolButton::clicked, this, [=]() {
        if(Config::getConfig<int>(Config::capture_mode) == Config::TOTAL_CAPTURE) {
            WindowManager::instance()->changeWindow("tray");
            QTimer::singleShot(200, this, [=]() {
                QPixmap map = ImageHelper::grabScreen();
                WindowManager::instance()->changeWindow("PaintWindow", map, ImageHelper::getCurrentScreen()->geometry());
            });
            return;
        } else {
            changeWindowHelper();
        }
    });
    new_button->setText(MString::search("{cR3jOHb9Qw}新建"));
    new_button->setIcon(ImageHelper::getIcon("capture"));
    new_button->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    addWidget(new_button);

    QToolButton* modeButton = new QToolButton(this);
    modeButton->setText(MString::search(MString::search("{7yUWnx82jI}模式")));
    modeButton->setIcon(ImageHelper::getIcon("mode"));
    modeButton->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    modeButton->setPopupMode(QToolButton::InstantPopup);
    QMenu* modeMenu = new QMenu(this);
    modeMenu->addAction(MString::search("{OBwjJUhTkh}矩形窗口"));
    modeMenu->addAction(MString::search("{FHFzLMcLYa}全屏"));
    modeMenu->addAction(MString::search("{fnGapBU4vo}自由截图"));
    modeMenu->addAction(MString::search("{ETY295cnab}滚动截屏"));
    QList<QAction*> actions = modeMenu->actions();
    for(int i=0; i<actions.size(); i++) {
        actions[i]->setCheckable(true);
        actions[i]->setData(QVariant(i));
    }
    actions[Config::getConfig<int>(Config::capture_mode)]->setChecked(true);
    connect(modeMenu, &QMenu::triggered, this, [=](QAction* action) {
        QVariant index_var = action->data();
        int index = index_var.toInt();
        for(int i=0; i<actions.size(); i++) {
            actions[i]->setChecked(false);
        }
        action->setChecked(true);
        Config::setConfig(Config::capture_mode, index);
    });
    modeButton->setMenu(modeMenu);
    addWidget(modeButton);

    GeoLayerContainer* geoLayerContainer = new GeoLayerContainer(area);
    addContainer("shape", "", geoLayerContainer, true);
    BlurLayerContainer* blurLayerContainer = new BlurLayerContainer(area);
    addContainer("mosaic", "", blurLayerContainer, true);
    QToolButton* cursorButton = new QToolButton(this);
    cursorButton->setIcon(ImageHelper::getIcon("cursor"));
    cursorButton->setToolTip(MString::search("{l4yTU9QXUd}指针"));
    addContainer(cursorButton,
        [=](){picLayer->setEnable(true, picLayer->type());attributeBar->removeAll();},
        [=](){picLayer->setEnable(false, picLayer->type());}, true);
    paintLayerContainer = new PaintLayerContainer(area);
    addContainer("pencil", "", paintLayerContainer, true);
    TextLayerContainer* textLayerContainer = new TextLayerContainer(area);
    addContainer("text", "", textLayerContainer, true);
    setContainer(paintLayerContainer);
    picLayer->setEnable(false, picLayer->type());

    QToolButton* eraseButton = new QToolButton(this);
    eraseButton->setIcon(ImageHelper::getIcon("eraser"));
    eraseButton->setToolTip(MString::search("{7cwKObEhcx}擦除"));
    addContainer(eraseButton,
        [=](){area->setEnable(false);area->setEraseEnable(true);attributeBar->removeAll();},
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

    QToolButton* ocrButton = new QToolButton(this);
    ocrButton->setIcon(ImageHelper::getIcon("ocr"));
    ocrButton->setToolTip(MString::search("{SvJhCjRGF0}提取文字"));
    connect(ocrButton, &QToolButton::clicked, this, [=]() {
        if(area->save(ILayerControl::Temp, "ocr/1.png")) {
            WindowManager::instance()->changeWindow("tray");
            ocrProcess.start();
        }
    });
    addWidget(ocrButton);

    QToolButton* clipButton = new QToolButton(this);
    clipButton->setIcon(ImageHelper::getIcon("clipboard"));
    clipButton->setToolTip(MString::search("{ntbJbEqxwF}复制到剪切板"));
    connect(clipButton, &QToolButton::clicked, this, [=]() {
        area->save(ILayerControl::ClipBoard, "");
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
                WindowManager::instance()->changeWindow("tray");
        }
    });
    addWidget(saveButton);

    QToolButton* pinButton = new QToolButton(this);
    pinButton->setToolTip(MString::search("{BNzMH1dcwW}钉住"));
    pinButton->setIcon(ImageHelper::getIcon("pin"));
    connect(pinButton, &QToolButton::clicked, this, [=](){
        QImage image = area->getSaveImage();
        WindowManager::instance()->changeWindow("PinWindow", QPixmap::fromImage(image), area->getSaveRect().toRect());
    });
    addWidget(pinButton);


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
    initOcr();
    hideAll();
}

void PaintToolbar::initOcr() {
    const QString queryCPUNumberOfCores = "wmic cpu get NumberOfCores";
    QProcess queryCPU;
    QPair<int, int> pairResult;
    queryCPU.start(queryCPUNumberOfCores);
    queryCPU.waitForFinished();
    QString result = QString::fromLocal8Bit(queryCPU.readAllStandardOutput());
    QStringList list = queryCPUNumberOfCores.split(" ");
    result = result.remove(list.last(), Qt::CaseInsensitive);
    result = result.replace("\r", "");
    result = result.replace("\n", "");
    result = result.simplified();
    pairResult.first = result.toInt();
    int numCore = pairResult.first;
    QStringList args;
    QDir dir("ocr/models");
    QDir dir2("ocr");
    args << "--models" << "models/"
         << "--det" << "ch_PP-OCRv3_det_infer.onnx"
         << "--cls" << "ch_ppocr_mobile_v2.0_cls_infer.onnx"
         << "--rec" << "ch_PP-OCRv3_rec_infer.onnx"
         << "--keys" << "ppocr_keys_v1.txt"
         << "--image" << "1.png"
         << "--numThread" << QString::number(numCore)
         << "--padding" << "50"
         << "--maxSideLen" << "1024"
         << "--boxScoreThresh" << "0.5"
         << "--boxThresh" << "0.3"
         << "--unClipRatio" << "1.6"
         << "--doAngle" << "1"
         << "--mostAngle" << "1";
    ocrProcess.setProgram(dir2.absolutePath() + "/RapidOcrOnnx.exe");
    ocrProcess.setArguments(args);
    ocrProcess.setWorkingDirectory(dir2.absolutePath());
    connect(&ocrProcess, static_cast<void (QProcess::*)(int, QProcess::ExitStatus)>(&QProcess::finished), this, [=](int exitCode, QProcess::ExitStatus exitStatus) {
        if(exitStatus == QProcess::NormalExit) {
            showOcrResultProcess.startDetached();
        }
    });
    showOcrResultProcess.setProgram("OcrViewer.exe");
    showOcrResultProcess.setWorkingDirectory(QDir::currentPath());
}

QWidget* PaintToolbar::getAttributeBar(){
    return attributeBar;
}

void PaintToolbar::reset(){
    LayerToolBar::reset();
    setContainer(paintLayerContainer);
}
