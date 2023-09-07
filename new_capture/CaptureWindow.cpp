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
    area = new PaintArea(this, true);
    area->stateChange(ARROW);
    view = new QGraphicsView(area, this);
    view->setStyleSheet(".QGraphicsView{background: transparent;border:0px;}");
    view->setViewportUpdateMode(QGraphicsView::FullViewportUpdate);
//    view->viewport()->installEventFilter(new GraphicsViewPatch(view));
    view->setFrameShape(QFrame::NoFrame);
    view->setBackgroundRole(QPalette::Light);
    view->setAlignment(Qt::AlignCenter);
    view->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    view->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    view->setSizeAdjustPolicy(QAbstractScrollArea::AdjustToContents);
    setCentralWidget(view);
    area->onViewSet(view);

    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, [=]() {
        if(!xHook->isMouseHookRunning() || xHook->uninstallMouseHook()) {
            timer->stop();
            WindowManager::closeWindow("CaptureWindow");
        }
    });
}

CaptureWindow::~CaptureWindow() {
//    delete ui;
    if(Config::getConfig<int>(Config::capture_mode) == Config::SCROLL_CAPTURE) {
        scroll_timer->stop();
    }
    timer->stop();
    delete timer;
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
    KeyManager::addFunc(this, name, "leave", [=](QObject* receiver, bool is_enter) {
        if(is_enter) {
            WindowManager::changeWindow("tray");
        }
    });
    KeyManager::addFunc(this, name, "capture_rect", [=](QObject* receiver, bool is_enter) {
        CaptureWindow* current = qobject_cast<CaptureWindow*>(receiver);
        if(current->area->hasFocus() && is_enter && !(Config::getConfig<int>(Config::capture_mode) == Config::SCROLL_CAPTURE))
            current->area->clipButtonEnter(0);
    });
    KeyManager::addFunc(this, name, "capture_mosaic", [=](QObject* receiver, bool is_enter) {
        CaptureWindow* current = qobject_cast<CaptureWindow*>(receiver);
        if(is_enter && !(Config::getConfig<int>(Config::capture_mode) == Config::SCROLL_CAPTURE))
            current->area->clipButtonEnter(1);
    });
    KeyManager::addFunc(this, name, "capture_cursor", [=](QObject* receiver, bool is_enter) {
        CaptureWindow* current = qobject_cast<CaptureWindow*>(receiver);
        if(is_enter && !(Config::getConfig<int>(Config::capture_mode) == Config::SCROLL_CAPTURE))
            current->area->clipButtonEnter(2);
    });
    KeyManager::addFunc(this, name, "capture_pencil", [=](QObject* receiver, bool is_enter) {
        CaptureWindow* current = qobject_cast<CaptureWindow*>(receiver);
        if(is_enter && !(Config::getConfig<int>(Config::capture_mode) == Config::SCROLL_CAPTURE))
            current->area->clipButtonEnter(3);
    });
    KeyManager::addFunc(this, name, "capture_highlighter", [=](QObject* receiver, bool is_enter) {
        CaptureWindow* current = qobject_cast<CaptureWindow*>(receiver);
        if(is_enter && !(Config::getConfig<int>(Config::capture_mode) == Config::SCROLL_CAPTURE))
            current->area->clipButtonEnter(4);
    });
    KeyManager::addFunc(this, name, "capture_text", [=](QObject* receiver, bool is_enter) {
        CaptureWindow* current = qobject_cast<CaptureWindow*>(receiver);
        if(is_enter && !(Config::getConfig<int>(Config::capture_mode) == Config::SCROLL_CAPTURE))
            current->area->clipButtonEnter(5);
    });
    KeyManager::addFunc(this, name, "capture_erase", [=](QObject* receiver, bool is_enter) {
        CaptureWindow* current = qobject_cast<CaptureWindow*>(receiver);
        if(is_enter && !(Config::getConfig<int>(Config::capture_mode) == Config::SCROLL_CAPTURE))
            current->area->clipButtonEnter(6);
    });
    KeyManager::addFunc(this, name, "capture_undo", [=](QObject* receiver, bool is_enter) {
        if(is_enter && !(Config::getConfig<int>(Config::capture_mode) == Config::SCROLL_CAPTURE))
            Recorder::instance()->back();
    });
    KeyManager::addFunc(this, name, "capture_redo", [=](QObject* receiver, bool is_enter) {
        if(is_enter && !(Config::getConfig<int>(Config::capture_mode) == Config::SCROLL_CAPTURE))
            Recorder::instance()->forward();
    });
    KeyManager::addFunc(this, name, "save2file", [=](QObject* receiver, bool is_enter) {
        CaptureWindow* current = qobject_cast<CaptureWindow*>(receiver);
        if(is_enter && !(Config::getConfig<int>(Config::capture_mode) == Config::SCROLL_CAPTURE)) {
            QString file_name = QFileDialog::getSaveFileName(this,
                                "保存",
                                History::instance()->get_last_directory(),
                                "图片(*.bmp *.jpg *.jpeg *.png);;所有文件(*)");
            KeyManager::clearKeys();
            if(file_name != "") {
                if(current->area->save(History_data::Persist, file_name))
                    WindowManager::changeWindow("tray");
            }
        }
    });
    KeyManager::addFunc(this, name, "save2clip", [=](QObject* receiver, bool is_enter) {
        CaptureWindow* current = qobject_cast<CaptureWindow*>(receiver);
        if(is_enter && !(Config::getConfig<int>(Config::capture_mode) == Config::SCROLL_CAPTURE)) {
            if(current->area->save2Clipboard())
                WindowManager::changeWindow("tray");
        }
    });
    KeyManager::addFunc(this, name, "enter_capture", [=](QObject* receiver, bool is_enter) {
        CaptureWindow* current = qobject_cast<CaptureWindow*>(receiver);
        if(is_enter && !(Config::getConfig<int>(Config::capture_mode) == Config::SCROLL_CAPTURE)) {
            current->area->sendRequestImage();
        }
    });
    KeyManager::addFunc(this, name, "capture_scrollrect", [=](QObject* receiver, bool is_enter) {
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
    KeyManager::unRegisterGlobalKey("capture_video_start");
    KeyManager::unRegisterGlobalKey("capture_video_pause");
    KeyManager::unRegisterGlobalKey("capture_video_stop");
}

void CaptureWindow::onWindowSelect() {
//    Window_fliter::instance()->SnapshotAllWinRect();
    KeyManager::registerGlobalKey("capture_video_start");
    KeyManager::registerGlobalKey("capture_video_pause");
    KeyManager::registerGlobalKey("capture_video_stop");
    area->reset();
    area->update();
    Flow_edit_panel::instance()->reset();
    Style_manager::instance()->reset();
    Recorder::instance()->reset();
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
