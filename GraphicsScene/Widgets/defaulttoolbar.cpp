#include "defaulttoolbar.h"
#include "../Container/blurlayercontainer.h"
#include "../Container/geolayercontainer.h"
#include "../Container/masklayercontainer.h"
#include "../Container/paintlayercontainer.h"
#include "../Container/textlayercontainer.h"
#include <QFileDialog>
#include "../../Manager/history.h"
#include <QProcess>

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

    QToolButton* ocrButton = new QToolButton(this);
    ocrButton->setIcon(ImageHelper::getIcon("ocr"));
    ocrButton->setToolTip(MString::search("{SvJhCjRGF0}提取文字"));
    connect(ocrButton, &QToolButton::clicked, this, [=]() {
        if(area->save(ILayerControl::Temp, "ocr/1.png")) {
            WindowManager::changeWindow("tray");
            ocrProcess.start();
        }
    });
    addWidget(ocrButton);

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

    QToolButton* pinButton = new QToolButton(this);
    pinButton->setToolTip(MString::search("{BNzMH1dcwW}钉住"));
    pinButton->setIcon(ImageHelper::getIcon("pin"));
    connect(pinButton, &QToolButton::clicked, this, [=](){
        QImage image = area->getSaveImage();
        WindowManager::changeWindow("PinWindow", QPixmap::fromImage(image), area->getSaveRect().toRect());
    });
    addWidget(pinButton);

    QToolButton* okButton = new QToolButton(this);
    okButton->setIcon(ImageHelper::getIcon("ok"));
    connect(okButton, &QToolButton::clicked, this, [=]() {
        QImage image = area->getSaveImage();
        QRect bound = area->getSaveRect().toRect();
        bound.moveTo(0, 0);
        WindowManager::changeWindow("PaintWindow", QPixmap::fromImage(image), bound);
    });
    addWidget(okButton);

    QToolButton* cancelButton = new QToolButton(this);
    cancelButton->setIcon(ImageHelper::getIcon("cancel"));
    connect(cancelButton, &QToolButton::clicked, this, [=]() {
        WindowManager::changeWindow("tray");
    });
    addWidget(cancelButton);


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

void DefaultToolbar::initOcr() {
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
