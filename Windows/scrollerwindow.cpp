#include "scrollerwindow.h"
#include "../Helper/imagehelper.h"
#include <QPainter>
#include <QWindow>
#include "../Manager/WindowManager.h"
#include "hook.h"
#include "windowfilter.h"
#include "../Helper/math.h"
#include <QMouseEvent>
#include "../Manager/KeyManager.h"
#include "../Manager/config.h"
ScrollerWindow::ScrollerWindow(QWidget* parent) : WindowBase(parent),
    scrollState(IDLE),
    beforeState(IDLE),
    isScrollRect(false),
    isScrollRectEnter(false),
    isScrollRectEnd(false),
    isScrollManual(false),
    end_scroll(false),
    ScrollerWindowIndex(0)
{
    setWindowFlag(Qt::FramelessWindowHint);
    setAttribute(Qt::WA_TranslucentBackground);
    setAttribute(Qt::WA_DeleteOnClose);
    setAttribute(Qt::WA_OpaquePaintEvent );
#ifdef QT_NO_DEBUG
    setWindowFlags(windowFlags() | Qt::WindowStaysOnTopHint);
#endif
    setMouseTracking(true);
    for(int i=0; i<QGuiApplication::screens().size(); i++){
        screenGeometry = screenGeometry.united(QGuiApplication::screens().at(i)->geometry());
    }
//    screenGeometry = ImageHelper::getCurrentScreen()->geometry();
    qDebug() << screenGeometry;
    setGeometry(screenGeometry);

    bubbleTipsWidget = new BubbleTipsWidget(this);
    QStringList tips;
    tips << "{yrlS5RFyAi}按Ctrl键可以自行设置滚动大小" << "{jjSL6uTMUW}在设置-捕获中可以调节滚动速度哦" << "{sI4UKVCDun}按鼠标中键手动滚动";
    bubbleTipsWidget->addContents(tips);
    bubbleTipsWidget->hide();
    intervalLabel = new FadeLabel("", this);
    intervalLabel->setFont(QFont("Roman", 20, 75));

    scrollState = IDLE;
    isScrollRect = false;
    isScrollRectEnter = false;
    isScrollRectEnd = false;
    dispatcher = new Scroll_dispatcher(this);
    bubbleTipsWidget->setFix(false);
    bubbleTipsWidget->show();

    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, [=]() {
        if(!xHook->isMouseHookRunning() || xHook->uninstallMouseHook()) {
            timer->stop();
            WindowManager::instance()->closeWindow("ScrollerWindow");
        }
    });

    connect(dispatcher, &Scroll_dispatcher::finish, this, [=](QImage image) {
        QPixmap pixmap;
        pixmap.convertFromImage(image);
        if(image.width() > 10 && image.height() > 10)
            WindowManager::instance()->changeWindow("PaintWindow", pixmap, QRect(0, 0, image.width(), image.height()));
        else  WindowManager::instance()->changeWindow("tray");
        if(!xHook->isMouseHookRunning() || xHook->uninstallMouseHook()) {
            WindowManager::instance()->closeWindow("ScrollerWindow");
        }
    });

    scrollTimer = new QTimer(this);
    scrollEndTimes = Config::instance()->getConfig<int>(Config::scroll_end_times);
    preImage.resize(scrollEndTimes+1);
    scrollEndDetectBegin = false;
    scrollTimes = 0;
    connect(scrollTimer, &QTimer::timeout, this, [=]() {
        QPixmap pix;
        pix = ImageHelper::grabScreen(ScrollerWindowIndex, activeWindowBound.x() + screenGeometry.x(),
                                      activeWindowBound.y() + screenGeometry.y(),
                                      activeWindowBound.width(), activeWindowBound.height());
        INPUT inputs[1] = {};
        ZeroMemory(inputs, sizeof(inputs));
        inputs[0].type = INPUT_MOUSE;
        inputs[0].mi.mouseData = -240;
        inputs[0].mi.dwFlags = MOUSEEVENTF_WHEEL;
        inputs[0].mi.dx = beginPoint.x();
        inputs[0].mi.dy = beginPoint.y();
        inputs[0].mi.time = 0;
        SendInput(ARRAYSIZE(inputs), inputs, sizeof(INPUT));
        QImage image = pix.toImage();
        bool success = false;
        if(pix.width() != activeWindowBound.width() ||
                pix.height() != activeWindowBound.height()){
            success = false;
        }
        if (scrollEndDetectBegin) {
            int lastIdx = (scrollTimes + 1) >= scrollEndTimes ? 0 : scrollTimes + 1;
            cv::Mat mat1 = ImageHelper::QImage2Mat(preImage[lastIdx]);
            if(mat1.cols != 0) {
                cv::Mat mat2 = ImageHelper::QImage2Mat(image);
                if(!ImageHelper::is_equal(mat1, mat2))success = true;
            } else {
                success = true;
            }
        }
        else {
            success = true;
        }
        if(!success || end_scroll) {
            dispatcher->get_all_images();//结束
            scrollTimer->stop();
            scrollState = SCROLL_END;
            update();
            return;
        }
        preImage[scrollTimes] = image;
        scrollTimes++;
        if (scrollTimes >= scrollEndTimes) {

            scrollTimes = 0;
            scrollEndDetectBegin = true;
        }

        //combine_image(pix.toImage());
//            image.save("F:/dinfo/" + QString::number(QDateTime::currentMSecsSinceEpoch()) + ".png");
        dispatcher->start(image);
    });
}

void ScrollerWindow::paintEvent(QPaintEvent* event){
    QPainter painter(this);
    if(scrollState != SCROLL_AUTO && scrollState != SCROLL_MANUAL && scrollState != SCROLL_END)
        painter.fillRect(QRect(0, 0, screenGeometry.width(), screenGeometry.height()), QColor(0, 0, 0, 1));

    QPen pen;
    pen.setColor(QColor(255, 0, 0));
    pen.setWidth(3);
    painter.setPen(pen);
    painter.setCompositionMode(QPainter::CompositionMode_Source);
    if(scrollState == SCROLL_AUTO || scrollState == SCROLL_MANUAL) {
            QRect rect = activeWindowBound;
            rect.setTopLeft(rect.topLeft() - QPoint(3, 3));
            rect.setBottomRight(rect.bottomRight() + QPoint(3, 3));
            painter.drawRect(rect);
            painter.drawText(activeWindowBound.left(),
                             activeWindowBound.top() + activeWindowBound.height()+20, MString::search("{NWZ0wFsHVM}按Esc中止\n请不要移动鼠标"));
    } else if(scrollState != SCROLL_END){
        painter.drawRect(activeWindowBound);
    }
}

void ScrollerWindow::mousePressEvent(QMouseEvent *event){
    beginPoint = event->pos();
    if((event->button() == Qt::LeftButton || event->button() == Qt::MidButton) &&
            scrollState != SCROLL_AUTO && scrollState != SCROLL_MANUAL && scrollState != SCROLL_END){
        // 手动设置滚动截屏区域
        if((scrollState == IDLE || scrollState == SCROLLRECT_SETTED) && isScrollRect) {
            scrollState = SCROLLRECT_SETTING;
        } else {
            bubbleTipsWidget->hide();
            beforeState = scrollState;
            ScrollerWindowIndex = ImageHelper::getCurrentIndex();
            if(event->button() == Qt::LeftButton) {
                scrollState = SCROLL_AUTO;
                if(xHook->installKeyHook()) {
                    connect(xHook, &XGlobalHook::keyEvent, this, [=](PKBDLLHOOKSTRUCT pKeyHookStruct) {
                        if(pKeyHookStruct->vkCode == KeyManager::instance()->nativeKeycode(Qt::Key_Escape)) {
                            while(!xHook->uninstallKeyHook() && xHook->isKeyHookRunning());
                            end_scroll = true;
                        }
                    });
                }
                update();
                POINT point;
                GetCursorPos(&point);
                scroll_hwnd = WindowFromPoint(point);
                int time = Config::getConfig<int>(Config::capture_interval);
                scrollTimer->start(time);
            } else {
                scrollState = SCROLL_MANUAL;
                lastCaptureTime = QDateTime::currentMSecsSinceEpoch();
                if(xHook->installKeyHook()) {
                    connect(xHook, &XGlobalHook::keyEvent, this, [=](PKBDLLHOOKSTRUCT pKeyHookStruct) {
                        if(pKeyHookStruct->vkCode == KeyManager::instance()->nativeKeycode(Qt::Key_Escape)) {
                            while(!xHook->uninstallKeyHook() && xHook->isKeyHookRunning());
                                dispatcher->get_all_images();//结束
                        }
                    });
                }

                if(xHook->installMouseHook()){
                    connect(xHook, &XGlobalHook::mouseEvent, this,
                    [=](XGlobalHook::button_type type, PMOUSEHOOKSTRUCT pMouseHookStruct, bool* is_shield) {
                        *is_shield = false;
                        if(type == XGlobalHook::MOUSE_WHEEL && scrollState == SCROLL_MANUAL) {
                            int time = Config::getConfig<int>(Config::capture_interval);
                            qint64 currentTime = QDateTime::currentMSecsSinceEpoch();
                            if(currentTime - lastCaptureTime > time) {
                                lastCaptureTime = currentTime;
                                QPixmap pix;
                                pix = ImageHelper::grabScreen(ScrollerWindowIndex, activeWindowBound.x() + screenGeometry.x(),
                                                              activeWindowBound.y() + screenGeometry.y(),
                                                              activeWindowBound.width(), activeWindowBound.height());
                                QImage image = pix.toImage();
                                dispatcher->start(image);
                            }
                        }
                    });
                }
                update();
                return;
            }
        }
    }
    else if(event->button() == Qt::RightButton && scrollState != SCROLL_AUTO && scrollState != SCROLL_MANUAL && scrollState != SCROLL_END){
        if(scrollState == SCROLLRECT_SETTED) {
            bubbleTipsWidget->setFix(false);
            scrollState = IDLE;
        } else {
            WindowManager::instance()->popWindow();
            timer->start(50);
        }
    }
}

void ScrollerWindow::mouseMoveEvent(QMouseEvent *event){
    currentPoint = event->pos();
    if(scrollState == IDLE){
        qint64 currentMoveTime = QDateTime::currentMSecsSinceEpoch();
        if(currentMoveTime - lastMoveTime > detectInterval){
            lastMoveTime = currentMoveTime;
            POINT point;
            POINT point2;
            point2.x = 0;
            point2.y = 0;
            GetCursorPos(&point);
            RECT rect2;
            HWND hwnd = Window_fliter::instance()->GetHWNDByPoint(QPoint(point.x, point.y));
            GetClientRect(hwnd, &rect2);
            ClientToScreen(hwnd, &point2);
            ScreenToClient(hwnd, &point);

            if(point.x < 0 || point.y < 0 || point.x > rect2.right || point.y > rect2.bottom) {
                int title_width = GetSystemMetrics(SM_CYCAPTION);
                point2.y -=  title_width;
                rect2.bottom += title_width;
            }
            activeWindowBound = QRect(QPoint(point2.x - screenGeometry.x(), point2.y - screenGeometry.y()),
                                      QSize(rect2.right, rect2.bottom));
            update();
        }
    }
    else if(scrollState == SCROLLRECT_SETTING){
        activeWindowBound = Math::buildRect(beginPoint, currentPoint).toRect();
        update();
    }
    if(scrollState != SCROLL_AUTO && scrollState != SCROLL_MANUAL && scrollState != SCROLL_END){
        bubbleTipsWidget->move(currentPoint + screenGeometry.topLeft());
    }
}

void ScrollerWindow::mouseReleaseEvent(QMouseEvent *event){
    if(event->button() == Qt::LeftButton && scrollState == SCROLLRECT_SETTING){
        scrollState = SCROLLRECT_SETTED;
        bubbleTipsWidget->setFixContent("{YZbdqyumbs}点击滚动区域进行滚动");
    }
}

void ScrollerWindow::onWindowCancel(){
    bubbleTipsWidget->hide();
}

void ScrollerWindow::onWindowSelect(){
    bubbleTipsWidget->show();
}

void ScrollerWindow::loadKeyEvent(QString name){
    KeyManager::instance()->addFunc(this, name, "scroll_scrollrect", [=](bool is_enter) {
        qDebug() << "scroll rect: " << is_enter;
        if(is_enter)
            isScrollRect = true;
        else if(!is_enter)
            isScrollRect = false;
    });
    KeyManager::instance()->addFunc(this, name, "scroll_leave", [=](bool is_enter) {
        if(is_enter) {
            if(scrollState == SCROLL_END){

            }
            else if(scrollState == SCROLL_AUTO && !xHook->isKeyHookRunning()) {
                end_scroll = true;
            } else if(scrollState == SCROLL_MANUAL && !xHook->isKeyHookRunning()) {
                dispatcher->get_all_images();//结束
            } else if(scrollState == SCROLLRECT_SETTED) {
                scrollState = IDLE;
            } else {
                WindowManager::instance()->changeWindow("tray");
            }
        }
    });
}

void ScrollerWindow::wheelEvent(QWheelEvent *event){
    int current = 0;
    int old = Config::getConfig<int>(Config::capture_interval);
    if(event->delta() > 0){
        Config::setConfig(Config::capture_interval, old+10);
        current = old + 10;
    }
    else{
        if(old > 50){
            current = old - 10;
        }
        else{
            current = old;
        }
        Config::setConfig(Config::capture_interval, current);
    }
    intervalLabel->move(event->pos() - QPoint(40, 40));
    intervalLabel->setText(QString::number(current));
    intervalLabel->showAni();
}
