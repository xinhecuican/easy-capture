#include "PaintArea.h"
#include "recorder.h"
#include "style_manager.h"
#include<QDir>
#include<QDateTime>
#include "history.h"
#include<QTimer>
#include "Helper/debug.h"
#include<QFileDialog>
#include<QMouseEvent>
#include "opencv2/core.hpp"
#include "opencv2/opencv.hpp"
#include "Helper/image_helper.h"
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include<QImageWriter>
#include "Helper/image_helper.h"
#include<QPixmapCache>
#include "Paint/Widgets/Layers/text_layer.h"
#include <QTextCodec>//转码
#include<QScrollBar>
#include "Paint/Widgets/Layers/rect_layer.h"
#include "Manager/config.h"
#include "Manager/WindowManager.h"
#include<QLabel>
#include<QGraphicsSceneMouseEvent>
#include "Panels/paint_setting_panel.h"
#include<QApplication>
#include<QClipboard>
#include <QScreen>
#include <QProcess>
#include "new_capture/Widgets/Scroll_handler/Scroll_handler_global.h"

PaintArea::PaintArea(QWidget* parent, bool enable_clip) : QGraphicsScene(parent) {
    is_save = false;
    is_clip = enable_clip;
    state = PAINT;
    shape_type = RECTANGLE;
    initProcess();
    if(!is_clip) {
        pic_layer = new Picture_layer();
        addItem(pic_layer);
        initSettingPanel();
        clip_layer = NULL;
    } else {
        pic_layer = NULL;
        clip_layer = new ClipLayer(parent, this);
        connect(clip_layer, &ClipLayer::requestImage, this, [=]() {
            prepareSave();
            QRectF bound = clip_layer->getClipRect();
            bound = bound.united(paint_layer->childrenBoundingRect());
            bound = bound.united(shape_layer->childrenBoundingRect());

            if(bound == QRectF(0, 0, 0, 0))
                return;

            QImage image(bound.width(), bound.height(), QImage::Format_ARGB32);
            image.fill(Qt::transparent);
            QPainter painter(&image);
            render(&painter, QRectF(QPointF(0, 0), bound.size()), bound);
            WindowManager::changeWindow("PaintWindow");
            bound.moveTo(0, 0);
            WindowManager::getWindow("PaintWindow")->setPic(QPixmap::fromImage(image), bound.toRect());
            endSave();
        });
        connect(clip_layer, &ClipLayer::needClip, this, [=]() {
            save2Clipboard();
            WindowManager::changeWindow("tray");
        });
        connect(clip_layer, &ClipLayer::needSave, this, [=]() {
            QString file_name = QFileDialog::getSaveFileName(parent,
                                "保存",
                                History::instance()->get_last_directory(),
                                "图片(*.bmp *.jpg *.jpeg *.png);;所有文件(*)");
            if(file_name != "") {
                if(save(History_data::Persist, file_name))
                    WindowManager::changeWindow("tray");
            }
        });
        connect(clip_layer, &ClipLayer::requestOcr, this, [=]() {
            if(save(History_data::Editable, "ocr/1.png")) {
                WindowManager::changeWindow("tray");
                startOcr();
            }
        });
        connect(clip_layer, &ClipLayer::paintShape, this, [=](SHAPE_TYPE type) {
            if(type == DELETE_SHAPE)
                deleteShape();
            else
                paintShape(type);
        });
        connect(clip_layer, &ClipLayer::stateChange, this, [=](PAINT_STATE state) {
            stateChange(state);
        });
        connect(clip_layer, &ClipLayer::mosaicChange, this, [=](bool is_range, int value) {
            shape_layer->changeBlur(is_range, value);
        });
        connect(clip_layer, &ClipLayer::needReset, this, [=]() {
            reset();
        });
        addItem(clip_layer);
    }
    paint_layer = new Paint_layer();
    paint_layer->setZValue(1);
    addItem(paint_layer);
    shape_layer = new ShapeLayer();
    shape_layer->setZValue(2);
    if(is_clip)
        shape_layer->reset();
    addItem(shape_layer);
}

void PaintArea::reset() {
    if(pic_layer != NULL) {
        pic_layer->reset();
    }
    if(paint_layer != NULL) {
        paint_layer->reset();
    }
    if(shape_layer != NULL) {
        shape_layer->reset();
    }
    if(clip_layer != NULL) {
        clip_layer->reset();
    }
    is_save = false;
}

void PaintArea::initProcess() {
    QStringList args;
    QDir dir("ocr/models");
    QDir dir2("ocr");
    args << "--models" << dir.absolutePath()
         << "--det" << "ch_PP-OCRv3_det_infer.onnx"
         << "--cls" << "ch_ppocr_mobile_v2.0_cls_infer.onnx"
         << "--rec" << "ch_PP-OCRv3_rec_infer.onnx"
         << "--keys" << "ppocr_keys_v1.txt"
         << "--image" << dir2.absoluteFilePath("1.png")
         << "--numThread" << QString::number(Scroll_handler_global::instance()->num_core)
         << "--padding" << "50"
         << "--maxSideLen" << "1024"
         << "--boxScoreThresh" << "0.5"
         << "--boxThresh" << "0.3"
         << "--unClipRatio" << "1.6"
         << "--doAngle" << "1"
         << "--mostAngle" << "1";
    ocrProcess.setProgram("ocr/RapidOcrOnnx.exe");
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

void PaintArea::setPic(QPixmap pic, QRect rect) {
    setSceneRect(0, 0, rect.width()*2, rect.height()*2);
    pic_layer->setPos(rect.width() / 2, rect.height() / 2);
    pic_layer->setPixmap(pic);
    shape_layer->setPic(pic, QPoint(pic.width() / 2, pic.height() / 2));
    for(QColor color : History::instance()->get_color()) {
        pic_layer->setSaveDisableColor(-1, color);
    }
    setSceneRect(0, 0, rect.width() * 2, rect.height() * 2);
}

void PaintArea::setClipPic(QPixmap pix) {
    clip_layer->setPic(pix);
    shape_layer->setPic(pix, QPoint(0, 0));
}

void PaintArea::mouseMoveEvent(QGraphicsSceneMouseEvent *event) {
    sendEvent(paint_layer, event);
    sendEvent(shape_layer, event);
    if(is_clip)
        sendEvent(clip_layer, event);
    QGraphicsScene::mouseMoveEvent(event);
}

void PaintArea::mousePressEvent(QGraphicsSceneMouseEvent *event) {
    sendEvent(paint_layer, event);
    sendEvent(shape_layer, event);
    if(is_clip)
        sendEvent(clip_layer, event);
    if(event->button() == Qt::BackButton) {
        Recorder::instance()->back();
    } else if(event->button() == Qt::ForwardButton) {
        Recorder::instance()->forward();
    }
    QGraphicsScene::mousePressEvent(event);
}

void PaintArea::mouseReleaseEvent(QGraphicsSceneMouseEvent *event) {
    sendEvent(paint_layer, event);
    sendEvent(shape_layer, event);
    if(is_clip)
        sendEvent(clip_layer, event);
    QGraphicsScene::mouseReleaseEvent(event);
}

void PaintArea::deleteShape() {
    shape_layer->deleteShape();
}

void PaintArea::paintShape(SHAPE_TYPE type) {
    shape_type = type;
    shape_layer->setShape(type);
    stateChange(SHAPE);
}

void PaintArea::setOtherLayer() {
    if(paint_layer == NULL && pic_layer != NULL) {
        paint_layer = new Paint_layer(pic_layer);
    } else if(paint_layer != NULL && pic_layer != NULL) {
        paint_layer->setParentItem(pic_layer);
    }
    if(shape_layer == NULL && pic_layer != NULL) {
        shape_layer = new ShapeLayer(pic_layer);
    } else if(shape_layer != NULL && pic_layer != NULL) {
        shape_layer->setParentItem(pic_layer);
    }
}

void PaintArea::stateChange(PAINT_STATE state) {
    if(state == this->state) {
        return;
    }
    // 放弃当前状态函数
    switch(this->state) {
    case ERASE:
        shape_layer->setEnable(false);
        paint_layer->setErase(false);
        break;
    case PAINT:
        paint_layer->setEnableDraw(false);
        break;
    case ARROW:
        if(!is_clip)
            pic_layer->setEnableMove(false);
        else {
            clip_layer->setEnable(false);
        }
        shape_layer->setGrabFocus(false);
        break;
    case SHAPE:
        shape_layer->setGrabFocus(false);
        shape_layer->setEnable(false);
        break;
    case MASK:
        break;
    }
    this->state = state;

    switch(state) {
    case PAINT:
        paint_layer->setEnableDraw(true);
        break;
    case ERASE:
        shape_layer->setEnable(true);
        shape_layer->setShape(DELETE_SHAPE);
        paint_layer->setErase(true);
        break;
    case ARROW:
        if(!is_clip)
            pic_layer->setEnableMove(true);
        else
            clip_layer->setEnable(true);
        shape_layer->setGrabFocus(true);
        break;
    case SHAPE:
        shape_layer->setEnable(true);
        shape_layer->setGrabFocus(true);
        break;
    }
}

void PaintArea::initSettingPanel() {
    connect(Paint_setting_panel::instance(), &Paint_setting_panel::disable_color_change, this,
    [=](int index, QColor color=QColor()) {
        if(pic_layer != NULL) {
            pic_layer->setDisableColor(index, color);
        }
    });
    connect(Paint_setting_panel::instance(), &Paint_setting_panel::saveDisableColorChange, this,
    [=](int index, QColor color=QColor()) {
        if(pic_layer != NULL) {
            pic_layer->setSaveDisableColor(index, color);
        }
    });
    connect(Paint_setting_panel::instance(), &Paint_setting_panel::layer_rename, this, [=](int index, QString after_name) {

    });
    connect(Paint_setting_panel::instance(), &Paint_setting_panel::remove_layer, this, [=](int index) {

    });
    connect(Paint_setting_panel::instance(), &Paint_setting_panel::change_layer_position,
    this, [=](int before_index, int after_index) {

    });
    connect(Paint_setting_panel::instance(), &Paint_setting_panel::requestLayersName, this, [=]() {
    });
}

bool PaintArea::save(History_data::save_type type, QString path) {
    if(path == "") {
        return false;
    }
    prepareSave();
    is_save = true;
    QRectF bound;
    if(pic_layer != NULL) {
        bound = pic_layer->boundingRect();
        bound.moveTo(pic_layer->pos() + bound.topLeft());
    } else {
        bound = clip_layer->getClipRect();
    }
    bound = bound.united(paint_layer->childrenBoundingRect());
    bound = bound.united(shape_layer->childrenBoundingRect());

    if(bound == QRectF(0, 0, 0, 0))
        return false;

    cv::Mat ans(bound.height(), bound.width(), CV_8UC4);
    for(int i=0; i<bound.height(); i+=32700) {
        int height = bound.height() - i > 32700 ? 32700 : bound.height() - i;
        QRect temp_rect(bound.left(), bound.top()+i, bound.width(), height);
        QImage image(bound.width(), height, QImage::Format_ARGB32);
//        image.fill(Qt::transparent);
        QPainter painter(&image);
        render(&painter, QRectF(QPointF(0, 0), image.size()), temp_rect);
        cv::Mat temp_mat = Image_helper::QImage2Mat(image);
        temp_mat.copyTo(ans(cv::Rect(0, i, bound.width(), height)));
    }
    cv::imwrite(path.toLocal8Bit().toStdString(), ans);
    History::instance()->log(type, path);
    endSave();
    return true;
}

bool PaintArea::save2Clipboard() {
    prepareSave();
    QRectF bound;
    if(pic_layer != NULL) {
        bound = pic_layer->boundingRect();
        bound.moveTo(pic_layer->pos() + bound.topLeft());
    } else {
        bound = clip_layer->getClipRect();
    }
    bound = bound.united(paint_layer->childrenBoundingRect());
    bound = bound.united(shape_layer->childrenBoundingRect());

    if(bound == QRectF(0, 0, 0, 0))
        return false;

    QImage image(bound.width(), bound.height(), QImage::Format_ARGB32);
    image.fill(Qt::transparent);
    QPainter painter(&image);
    render(&painter, QRectF(QPointF(0, 0), bound.size()), bound);
    QClipboard *clip=QApplication::clipboard();
    clip->setImage(image);
    endSave();
    return true;
}

bool PaintArea::needSave() {
    return pic_layer != NULL && pic_layer->containsPicture() && !is_save;
}

void PaintArea::prepareSave() {
    if(pic_layer != NULL)
        pic_layer->prepareSave();
    else
        clip_layer->prepareSave();
    shape_layer->prepareSave();
    update();
}

void PaintArea::endSave() {
    if(pic_layer != NULL)
        pic_layer->endSave();
    else
        clip_layer->endSave();
    shape_layer->endSave();
    update();
}

void PaintArea::sendRequestImage() {
    if(clip_layer != NULL)
        clip_layer->sendRequestImage();
}

void PaintArea::onViewSet(QWidget* view) {
    if(clip_layer != NULL)
        clip_layer->setWidgetParent(view);
}

void PaintArea::clipButtonEnter(int id) {
    if(clip_layer != NULL)
        clip_layer->buttonEnter(id);
}

void PaintArea::startOcr() {
    ocrProcess.start();
}

RecordInfo PaintArea::getRecordInfo() {
    if(clip_layer != NULL)
        return clip_layer->getRecordInfo();
    return RecordInfo();
}
