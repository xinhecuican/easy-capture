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
    scroll_image = QImage();
    pre_image = QImage();
    mouse_move_times = 0;
    qRegisterMetaType<cv::Mat>("cv::Mat");

    is_finish = false;
    begin_waiting = false;
    window_valid = false;
    videoCapture = new VideoCaptureHandler(this);
    isVideoCapture = false;
    isScrollManual = false;
    scrollState = IDLE;
    beforeState = IDLE;

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
    setGeometry(QGuiApplication::primaryScreen()->geometry());
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
    bubbleTipsWidget = new BubbleTipsWidget(this);
    QStringList tips;
    tips << "{yrlS5RFyAi}按Ctrl键可以自行设置滚动大小" << "{jjSL6uTMUW}在设置-捕获中可以调节滚动速度哦" << "{sI4UKVCDun}按鼠标中键手动滚动";
    bubbleTipsWidget->addContents(tips);
    bubbleTipsWidget->hide();

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
    if(Config::getConfig<int>(Config::capture_mode) == Config::SCROLL_CAPTURE) {
        QPen pen;
        pen.setColor(QColor(255, 0, 0));
        pen.setWidth(3);
        painter.setPen(pen);
        painter.setCompositionMode(QPainter::CompositionMode_Source);
        if(scrollState == SCROLL_AUTO || scrollState == SCROLL_MANUAL) {
            if(window_valid && beforeState != SCROLLRECT_SETTED) {
                QRect rect = active_window_bound;
                painter.drawRect(rect);
                painter.drawText(active_window_bound.left(),
                                 active_window_bound.top() + active_window_bound.height()-20, "按Esc中止");
            } else {
                QRect rect = active_window_bound;
                rect.setTopLeft(rect.topLeft() - QPoint(3, 3));
                rect.setBottomRight(rect.bottomRight() + QPoint(3, 3));
                painter.drawRect(rect);
                painter.drawText(active_window_bound.left(),
                                 active_window_bound.top() + active_window_bound.height()+20, MString::search("{NWZ0wFsHVM}按Esc中止\n请不要移动鼠标"));
            }
        } else {
            painter.drawRect(active_window_bound);
        }
        return;
    } else if(Config::getConfig<int>(Config::capture_mode) == Config::RECT_CAPTURE && isVideoCapture) {
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
            if(Config::getConfig<int>(Config::capture_mode) == Config::SCROLL_CAPTURE) {
                if(scrollState == SCROLL_AUTO && !xHook->isKeyHookRunning()) {
                    end_scroll = true;
                } else if(scrollState == SCROLL_MANUAL && !xHook->isKeyHookRunning()) {
                    dispatcher->get_all_images();//结束
                } else if(scrollState == SCROLLRECT_SETTED) {
                    scrollState = IDLE;
                } else {
                    WindowManager::changeWindow("tray");
                }
            } else {
                WindowManager::changeWindow("tray");
            }
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
        CaptureWindow* current = qobject_cast<CaptureWindow*>(receiver);
        if(is_enter && Config::getConfig<int>(Config::capture_mode) == Config::SCROLL_CAPTURE)
            current->isScrollRect = true;
        else if(!is_enter)
            current->isScrollRect = false;
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
    bubbleTipsWidget->hide();
//    Style_manager::instance()->reset();
//    Recorder::instance()->reset();
    KeyManager::unRegisterGlobalKey("capture_video_start");
    KeyManager::unRegisterGlobalKey("capture_video_pause");
    KeyManager::unRegisterGlobalKey("capture_video_stop");
}

enum window_search_mode {
    INCLUDE_MINIMIZED,
    EXCLUDE_MINIMIZED
};
static bool check_window_valid(HWND window, enum window_search_mode mode) {
    DWORD styles, ex_styles;
    RECT  rect;

    if (/*!IsWindowVisible(window) ||*/
        (mode == EXCLUDE_MINIMIZED && IsIconic(window)))
        return false;
    GetClientRect(window, &rect);
    styles    = (DWORD)GetWindowLongPtr(window, GWL_STYLE);
    ex_styles = (DWORD)GetWindowLongPtr(window, GWL_EXSTYLE);

    if (ex_styles & WS_EX_TOOLWINDOW)
        return false;
    if (styles & WS_CHILD)
        return false;
    if (mode == EXCLUDE_MINIMIZED && (rect.bottom == 0 || rect.right == 0))
        return false;
    return true;
}

static inline HWND next_window(HWND window, enum window_search_mode mode) {
    while (true) {
        window = GetNextWindow(window, GW_HWNDNEXT);
        if (!window || check_window_valid(window, mode))
            break;
    }

    return window;
}

void CaptureWindow::onWindowSelect() {
//    Window_fliter::instance()->SnapshotAllWinRect();
    KeyManager::registerGlobalKey("capture_video_start");
    KeyManager::registerGlobalKey("capture_video_pause");
    KeyManager::registerGlobalKey("capture_video_stop");
    if(Config::getConfig<int>(Config::capture_mode) == Config::SCROLL_CAPTURE) {
        is_enter = false;
        set_scroll_info();
        view->hide();
        if(xHook->installMouseHook()) {
            // is_shield: 屏蔽鼠标事件
            connect(xHook, &XGlobalHook::mouseEvent, this,
            [=](XGlobalHook::button_type type, PMOUSEHOOKSTRUCT pMouseHookStruct, bool* is_shield) {
                *is_shield = false;
                if(type == XGlobalHook::MOUSE_MOVE && (scrollState != SCROLL_AUTO && scrollState != SCROLL_MANUAL)) {
                    POINT point;
                    GetCursorPos(&point);
                    end_point.setX(point.x);
                    end_point.setY(point.y);
                    bubbleTipsWidget->move(end_point);
                }
                if(type == XGlobalHook::MOUSE_WHEEL && scrollState == SCROLL_MANUAL) {
                    int time = Config::getConfig<int>(Config::capture_interval);
                    qint64 currentTime = QDateTime::currentMSecsSinceEpoch();
                    if(currentTime - lastCaptureTime > time) {
                        lastCaptureTime = currentTime;
                        QScreen * screen = QGuiApplication::primaryScreen();
                        QPixmap pix = ImageHelper::grabScreen(active_window_bound.x(), active_window_bound.y(),
                                                              active_window_bound.width(), active_window_bound.height());
                        QImage image = pix.toImage();
                        dispatcher->start(image);
                    }
                }
                if(type == XGlobalHook::MOUSE_MOVE && scrollState == IDLE) {
                    POINT point;
                    POINT point2;
                    point2.x = 0;
                    point2.y = 0;
                    GetCursorPos(&point);
                    RECT rect2;
                    HWND hwnd = WindowFromPoint(point);
                    GetClientRect(hwnd, &rect2);
                    ClientToScreen(hwnd, &point2);
                    ScreenToClient(hwnd, &point);

                    if(point.x < 0 || point.y < 0 || point.x > rect2.right || point.y > rect2.bottom) {
                        int title_width = GetSystemMetrics(SM_CYCAPTION);
                        point2.y -=  title_width;
                        rect2.bottom += title_width;
                    }
                    active_window_bound = QRect(QPoint(point2.x, point2.y), QSize(rect2.right, rect2.bottom));
                    update();
                } else if(type == XGlobalHook::MOUSE_MOVE && scrollState == SCROLLRECT_SETTING) {
                    active_window_bound = Math::buildRect(cursor_point, end_point).toRect();
                    update();
                } else if(type == XGlobalHook::LBUTTON_UP && scrollState == SCROLLRECT_SETTING) {
                    scrollState = SCROLLRECT_SETTED;
                    bubbleTipsWidget->setFixContent("{YZbdqyumbs}点击滚动区域进行滚动");
                } else if((type == XGlobalHook::LBUTTON || type == XGlobalHook::MBUTTON) && scrollState != SCROLL_AUTO && scrollState != SCROLL_MANUAL) {
                    end_scroll = false;
                    *is_shield = true;
                    POINT point;
                    POINT point2;
                    point2.x = 0;
                    point2.y = 0;
                    GetCursorPos(&point);
                    cursor_point.setX(point.x);
                    cursor_point.setY(point.y);
                    // 手动设置滚动截屏区域
                    if((scrollState == IDLE || scrollState == SCROLLRECT_SETTED) && isScrollRect) {
                        scrollState = SCROLLRECT_SETTING;
                    } else {
                        bubbleTipsWidget->hide();
                        beforeState = scrollState;
                        if(type == XGlobalHook::LBUTTON) {
                            scrollState = SCROLL_AUTO;
                            if(xHook->installKeyHook()) {
                                connect(xHook, &XGlobalHook::keyEvent, this, [=](PKBDLLHOOKSTRUCT pKeyHookStruct) {
                                    if(pKeyHookStruct->vkCode == KeyManager::nativeKeycode(Qt::Key_Escape)) {
                                        while(!xHook->uninstallKeyHook() && xHook->isKeyHookRunning());
                                        end_scroll = true;
                                    }
                                });
                            }
                        } else {
                            scrollState = SCROLL_MANUAL;
                            window_valid = false;
                            lastCaptureTime = QDateTime::currentMSecsSinceEpoch();
                            if(xHook->installKeyHook()) {
                                connect(xHook, &XGlobalHook::keyEvent, this, [=](PKBDLLHOOKSTRUCT pKeyHookStruct) {
                                    if(pKeyHookStruct->vkCode == KeyManager::nativeKeycode(Qt::Key_Escape)) {
                                        while(!xHook->uninstallKeyHook() && xHook->isKeyHookRunning());
                                        dispatcher->get_all_images();//结束
                                    }
                                });
                            }
                            update();
                            return;
                        }
                        scroll_hwnd = WindowFromPoint(point);
                        scrollWindowIndex = ImageHelper::getCurrentIndex();
                        QPixmap pix = ImageHelper::grabScreen(scrollWindowIndex, WId(scroll_hwnd));
                        QImage image = pix.toImage();
                        //                    image.save("f:/dinfo/temp.png");
                        for(int i=0; i<image.height(); i+=5) {
                            for(int k=0; k<image.width(); k+=5) {
                                if(image.pixel(k, i) != 0xff000000) {
                                    window_valid = true;
                                    goto WINDOW_VALID_OUT;
                                }
                            }
                        }
                        window_valid = false;

WINDOW_VALID_OUT:
                        ;
                        update();
                        int time = Config::getConfig<int>(Config::capture_interval);
                        scroll_timer->start(time);
                    }
                } else if(type == XGlobalHook::RBUTTON && scrollState != SCROLL_AUTO && scrollState != SCROLL_MANUAL) {
                    *is_shield = true;
                    if(scrollState == SCROLLRECT_SETTED) {
                        bubbleTipsWidget->setFix(false);
                        scrollState = IDLE;
                    } else {
                        WindowManager::popWindow();
                        timer->start(50);
                    }
                }
            });
        }
    } else {
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
}

void CaptureWindow::set_scroll_info() {
    if(Config::getConfig<int>(Config::capture_mode) == Config::SCROLL_CAPTURE) {
        scrollState = IDLE;
        isScrollRect = false;
        isScrollRectEnter = false;
        isScrollRectEnd = false;
        dispatcher = new Scroll_dispatcher(this);
        bubbleTipsWidget->setFix(false);
        bubbleTipsWidget->show();
        connect(dispatcher, &Scroll_dispatcher::finish, this, [=](QImage image) {
            WindowManager::changeWindow("PaintWindow");
            QPixmap pixmap;
            pixmap.convertFromImage(image);
            WindowManager::getWindow("PaintWindow")->
            setPic(pixmap, QRect(0, 0, image.width(), image.height()));
            timer->start(50);

        });
        scroll_timer = new QTimer(this);
        connect(scroll_timer, &QTimer::timeout, this, [=]() {
            QScreen * screen = QGuiApplication::primaryScreen();
            QPixmap pix;
            if(!window_valid || beforeState == SCROLLRECT_SETTED) {
                pix = ImageHelper::grabScreen(scrollWindowIndex, active_window_bound.x(), active_window_bound.y(),
                                              active_window_bound.width(), active_window_bound.height());
            } else {
                pix = ImageHelper::grabScreen(scrollWindowIndex, WId(scroll_hwnd));
            }

            QWindow* mainWindow = QWindow::fromWinId(WId(scroll_hwnd));
            QPoint window_point;
            if (mainWindow != nullptr) {
                window_point = mainWindow->framePosition();
            }
            if(false) {
                INPUT inputs[1] = {};
                ZeroMemory(inputs, sizeof(inputs));
                inputs[0].type = INPUT_MOUSE;
                inputs[0].mi.mouseData = -240;
                inputs[0].mi.dwFlags = MOUSEEVENTF_WHEEL;
                inputs[0].mi.dx = 0;
                inputs[0].mi.dy = 0;
                inputs[0].mi.time = 0;
                SendInput(ARRAYSIZE(inputs), inputs, sizeof(INPUT));
            } else {
                PostMessage(scroll_hwnd, WM_MOUSEWHEEL, MAKEWPARAM(0, -240), MAKELPARAM(cursor_point.x(),
                            cursor_point.y()));
            }
            QImage image = pix.toImage();

            bool success = false;
            cv::Mat mat1 = ImageHelper::QImage2Mat(pre_image);
            if(mat1.cols != 0) {
                cv::Mat mat2 = ImageHelper::QImage2Mat(image);
                if(!ImageHelper::is_equal(mat1, mat2))success = true;
            } else {
                success = true;
            }
            if(!success || end_scroll) {
                dispatcher->get_all_images();//结束
                scroll_timer->stop();
                return;
            }
            pre_image = image;

            //combine_image(pix.toImage());
//            image.save("F:/dinfo/" + QString::number(QDateTime::currentMSecsSinceEpoch()) + ".png");
            dispatcher->start(image);
        });
    }
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
