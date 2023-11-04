#include "CaptureWindow.h"
#include "Manager/WindowManager.h"
#include<QDebug>
#include<QPainter>
#include<QPen>
#include<QDesktopWidget>
#include "Manager/config.h"
#include "Manager/KeyManager.h"
#include<QScreen>
#include "hook.h"
#include<QWindow>
#include "window_fliter.h"
#include "Helper/imagehelper.h"
#include<malloc.h>
#include<QBitmap>
#include<stdio.h>
#include <QGraphicsView>
#include "Paint/Widgets/PaintArea.h"
#include "Helper/GraphicsViewPatch.h"
#include <QFileDialog>
#include "Paint/Widgets/history.h"
#include "Paint/Widgets/style_manager.h"
#include "Paint/Widgets/recorder.h"
#include "Paint/Widgets/Panels/flow_edit_panel.h"
#include "Helper/math.h"
#include <QGuiApplication>
#include <QGraphicsProxyWidget>
#include "Widgets/capturetip.h"

bool CaptureWindow::end_scroll = false;

CaptureWindow::CaptureWindow(QWidget *parent) :
    WindowBase(parent)
//    ui(new Ui::CaptureWindow)
{
//    ui->setupUi(this);
    centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);
    button_click = false;
    is_first_capture = false;//自由截屏使用，已废弃
    mouse_move_times = 0;

    is_finish = false;
    begin_waiting = false;
    videoCapture = new VideoCaptureHandler(this);
    isVideoCapture = false;

    setWindowFlag(Qt::FramelessWindowHint);
    setAttribute(Qt::WA_TranslucentBackground);
    setAttribute(Qt::WA_DeleteOnClose);
    setAttribute(Qt::WA_OpaquePaintEvent );
//    showFullScreen();
    this->setMouseTracking(true);
    centralWidget->setMouseTracking(true);
#ifdef QT_NO_DEBUG
    setWindowFlags(windowFlags() | Qt::WindowStaysOnTopHint);
#endif

//    ui->centralwidget->setGeometry(QGuiApplication::primaryScreen()->geometry());
    setGeometry(ImageHelper::getCurrentScreen()->geometry());
    view = new QGraphicsView(this);
    area = new PaintArea(view, true);
    area->stateChange(ARROW);

    view->setScene(area);
    view->setStyleSheet(".QGraphicsView{background: transparent;border:0px;}");
    view->setViewportUpdateMode(QGraphicsView::FullViewportUpdate);
//    view->viewport()->installEventFilter(new GraphicsViewPatch(view));
    view->setFrameShape(QFrame::NoFrame);
    view->setBackgroundRole(QPalette::Light);
    view->setAlignment(Qt::AlignCenter);
    view->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    view->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    view->setSizeAdjustPolicy(QAbstractScrollArea::AdjustToContents);
    if((Config::getConfig<int>(Config::show_guidance) & Config::CAPTURE_GUID) == 0){
        tips = new BubbleTipsWidget(this);
        tips->setBackColor(255, 255, 255);
        tips->setShowHideButton(true);
        CaptureTip* captureTip = new CaptureTip(this);
        tips->setContent(captureTip);
        connect(tips, &BubbleTipsWidget::hideButtonTrigger, this, [=](bool trigger){
            int guidance = Config::getConfig<int>(Config::show_guidance);
            if(trigger)
                guidance = guidance | Config::CAPTURE_GUID;
            else
                guidance = guidance & (~Config::CAPTURE_GUID);
            Config::setConfig(Config::show_guidance, guidance);
            Config::updateConfig(Config::show_guidance);
        });
        QGraphicsProxyWidget* proxyTips = area->addTip(tips);
        proxyTips->setZValue(10);
    }

    setCentralWidget(view);
//    area->onViewSet(view);
}

CaptureWindow::~CaptureWindow() {
//    delete ui
    if(xHook->isMouseHookRunning()) {
        xHook->uninstallMouseHook();
    }
    if(xHook->isKeyHookRunning()) {
        xHook->uninstallKeyHook();
    }
}

void CaptureWindow::paintEvent(QPaintEvent *paint_event) {
    QPainter painter(this);
//    if(Config::getConfig<bool>(Config::free_capture) && button_click)
//    {
//        QPen pen;
//        pen.setStyle(Qt::DashLine);
//        pen.setColor(QColor(123, 123, 233));
//        pen.setWidth(3);
//        painter.setPen(pen);
//        painter.drawPath(free_paint_path);
//        return;
//    }
    if(Config::getConfig<int>(Config::capture_mode) == Config::RECT_CAPTURE && isVideoCapture) {
        QPen pen;
        pen.setWidth(3);
        if(videoCapture->getIsPause()) {
            pen.setColor(QColor(247,186,11));
        } else {
            pen.setColor(QColor(255, 0, 0));
        }
        painter.setPen(pen);
        QRect rect = videoCapture->getBound();
        rect.setTopLeft(rect.topLeft() - QPoint(3, 3));
        rect.setBottomRight(rect.bottomRight() + QPoint(3, 3));
        painter.drawRect(rect);
    }
}

void CaptureWindow::loadKeyEvent(QString name) {
//    if(!KeyManager::isContainsWindow(name))
//    {
KeyManager::instance()->addFunc(this, name, "leave", [=](bool is_enter) {
        if(is_enter) {
            WindowManager::changeWindow("tray");
        }
    });
    KeyManager::instance()->addFunc(this, name, "capture_rect", [=](bool is_enter) {
        if(area->hasFocus() && is_enter && !(Config::getConfig<int>(Config::capture_mode) == Config::SCROLL_CAPTURE))
            area->clipButtonEnter(0);
    });
    KeyManager::instance()->addFunc(this, name, "capture_mosaic", [=](bool is_enter) {
        if(is_enter && !(Config::getConfig<int>(Config::capture_mode) == Config::SCROLL_CAPTURE))
            area->clipButtonEnter(1);
    });
    KeyManager::instance()->addFunc(this, name, "capture_cursor", [=](bool is_enter) {
        if(is_enter && !(Config::getConfig<int>(Config::capture_mode) == Config::SCROLL_CAPTURE))
            area->clipButtonEnter(2);
    });
    KeyManager::instance()->addFunc(this, name, "capture_pencil", [=](bool is_enter) {
        if(is_enter && !(Config::getConfig<int>(Config::capture_mode) == Config::SCROLL_CAPTURE))
            area->clipButtonEnter(3);
    });
    KeyManager::instance()->addFunc(this, name, "capture_highlighter", [=](bool is_enter) {
        if(is_enter && !(Config::getConfig<int>(Config::capture_mode) == Config::SCROLL_CAPTURE))
            area->clipButtonEnter(4);
    });
    KeyManager::instance()->addFunc(this, name, "capture_text", [=](bool is_enter) {
        if(is_enter && !(Config::getConfig<int>(Config::capture_mode) == Config::SCROLL_CAPTURE))
            area->clipButtonEnter(5);
    });
    KeyManager::instance()->addFunc(this, name, "capture_erase", [=](bool is_enter) {
        if(is_enter && !(Config::getConfig<int>(Config::capture_mode) == Config::SCROLL_CAPTURE))
            area->clipButtonEnter(6);
    });
    KeyManager::instance()->addFunc(this, name, "capture_undo", [=](bool is_enter) {
        if(is_enter && !(Config::getConfig<int>(Config::capture_mode) == Config::SCROLL_CAPTURE))
            Recorder::instance()->back();
    });
    KeyManager::instance()->addFunc(this, name, "capture_redo", [=](bool is_enter) {
        if(is_enter && !(Config::getConfig<int>(Config::capture_mode) == Config::SCROLL_CAPTURE))
            Recorder::instance()->forward();
    });
    KeyManager::instance()->addFunc(this, name, "save2file", [=](bool is_enter) {
        if(is_enter && !(Config::getConfig<int>(Config::capture_mode) == Config::SCROLL_CAPTURE)) {
            QString file_name = QFileDialog::getSaveFileName(this,
                                "保存",
                                History::instance()->get_last_directory(),
                                "图片(*.bmp *.jpg *.jpeg *.png);;所有文件(*)");
            KeyManager::instance()->clearKeys("CaptureWindow");
            if(file_name != "") {
                if(area->save(History_data::Persist, file_name))
                    WindowManager::changeWindow("tray");
            }
        }
    });
    KeyManager::instance()->addFunc(this, name, "save2clip", [=](bool is_enter) {
        if(is_enter && !(Config::getConfig<int>(Config::capture_mode) == Config::SCROLL_CAPTURE)) {
            if(area->save2Clipboard())
                WindowManager::changeWindow("tray");
        }
    });
    KeyManager::instance()->addFunc(this, name, "enter_capture", [=](bool is_enter) {
        if(is_enter && !(Config::getConfig<int>(Config::capture_mode) == Config::SCROLL_CAPTURE)) {
            area->sendRequestImage();
        }
    });
    KeyManager::instance()->addFunc(this, name, "capture_scrollrect", [=](bool is_enter) {
    });
//    }
}

void CaptureWindow::mouseMoveEvent(QMouseEvent *event) {
    now_point = event->pos();
    mouse_move_times++;
    if(Config::getConfig<int>(Config::capture_mode) == Config::SCROLL_CAPTURE) {
        return;
    }
}

void CaptureWindow::mousePressEvent(QMouseEvent *event) {
    if(Config::getConfig<int>(Config::capture_mode) == Config::SCROLL_CAPTURE) {
        return;
    }
}

void CaptureWindow::mouseReleaseEvent(QMouseEvent *event) {
    if(Config::getConfig<int>(Config::capture_mode) == Config::SCROLL_CAPTURE) {
        return;
    }
}

void CaptureWindow::onWindowCancel() {
    button_click = false;
//    is_first_capture = true;
//    area->reset();
    free_paint_path = QPainterPath();
    active_window_bound = QRect();
//    Style_manager::instance()->reset();
//    Recorder::instance()->reset();
    KeyManager::instance()->unRegisterGlobalKey("capture_video_start");
    KeyManager::instance()->unRegisterGlobalKey("capture_video_pause");
    KeyManager::instance()->unRegisterGlobalKey("capture_video_stop");
}

void CaptureWindow::onWindowSelect() {
//    Window_fliter::instance()->SnapshotAllWinRect();
    KeyManager::instance()->registerGlobalKey("capture_video_start");
    KeyManager::instance()->registerGlobalKey("capture_video_pause");
    KeyManager::instance()->registerGlobalKey("capture_video_stop");
    area->reset();
    area->update();
    Flow_edit_panel::instance()->reset();
    Style_manager::instance()->reset();
    Recorder::instance()->reset();
    setGeometry(ImageHelper::getCurrentGeometry());
    view->show();
    QPixmap p = ImageHelper::grabScreen();
    area->setClipPic(p);
    area->stateChange(ARROW);
}

void CaptureWindow::startCaptureVideo() {
    if(!(Config::getConfig<int>(Config::capture_mode) == Config::SCROLL_CAPTURE)) {
        videoCapture->setCaptureInfo(area->getRecordInfo());
        if(videoCapture->isValid() && !isVideoCapture) {
            isVideoCapture = true;
            view->hide();
            update();
            videoCapture->startCapture();
        }
        qInfo() << "开始录屏";
    }
}

void CaptureWindow::pauseCaptureVideo() {
    if(!(Config::getConfig<int>(Config::capture_mode) == Config::SCROLL_CAPTURE) && videoCapture->isValid() && isVideoCapture) {
        videoCapture->pauseOrResume();
        update();
    }
}

void CaptureWindow::stopCaptureVideo() {
    if(!(Config::getConfig<int>(Config::capture_mode) == Config::SCROLL_CAPTURE) && videoCapture->isValid() && isVideoCapture) {
        isVideoCapture = false;
        videoCapture->stopCapture();
        WindowManager::changeWindow("tray");
        view->show();
    }
}
