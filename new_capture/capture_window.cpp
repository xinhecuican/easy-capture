#include "capture_window.h"
#include "ui_capture_window.h"
#include "Manager/window_manager.h"
#include<QDebug>
#include<QPainter>
#include<QPen>
#include<QDesktopWidget>
#include "Manager/config.h"
#include "Manager/key_manager.h"
#include<QScreen>
#include "hook.h"
#include<QWindow>
#include "window_fliter.h"
#include "Helper/image_helper.h"
#include<malloc.h>
#include<QBitmap>
#include<stdio.h>
bool Capture_window::end_scroll = false;

Capture_window::Capture_window(QWidget *parent) :
    Window_base(parent, this, "Capture_window"),
    ui(new Ui::Capture_window)
{
    ui->setupUi(this);
    button_click = false;
    is_first_capture = false;//自由截屏使用，已废弃
    scroll_image = QImage();
    pre_image = QImage();
    mouse_move_times = 0;
    qRegisterMetaType<cv::Mat>("cv::Mat");

    is_finish = false;
    begin_waiting = false;
    window_valid = false;

    setWindowFlag(Qt::FramelessWindowHint);
    setAttribute(Qt::WA_TranslucentBackground);
    setAttribute(Qt::WA_DeleteOnClose);
    showFullScreen();
    this->setMouseTracking(true);
    this->ui->centralwidget->setMouseTracking(true);
    setWindowFlags(windowFlags() | Qt::WindowStaysOnTopHint);

    this->captured = new Capture_area(this);
    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, [=](){
        if(!xHook->isMouseHookRunning() || xHook->uninstallMouseHook())
        {
            timer->stop();
            Window_manager::close_window("Capture_window");
        }
    });
    load_key_event("Capture_window");
}

Capture_window::~Capture_window()
{
    delete ui;
    if(Config::getConfig<bool>(Config::scroll_capture))
    {
        delete dispatcher;
        scroll_timer->stop();
        delete scroll_timer;
    }
    timer->stop();
    delete timer;
    if(xHook->isMouseHookRunning())
    {
        xHook->uninstallMouseHook();
    }
}

void Capture_window::paintEvent(QPaintEvent *paint_event)
{
    QPainter painter(this);
    if(Config::getConfig<bool>(Config::free_capture) && button_click)
    {
        QPen pen;
        pen.setStyle(Qt::DashLine);
        pen.setColor(QColor(123, 123, 233));
        pen.setWidth(3);
        painter.setPen(pen);
        painter.drawPath(free_paint_path);
        return;
    }
    else if(Config::getConfig<bool>(Config::scroll_capture))
    {
        QPen pen;
        pen.setColor(QColor(255, 0, 0));
        pen.setWidth(3);
        painter.setPen(pen);
        painter.setCompositionMode(QPainter::CompositionMode_Source);
        if(is_enter)
        {
            if(window_valid)
            {
                QRect rect = active_window_bound;
                painter.drawRect(rect);
                painter.drawText(active_window_bound.left(),
                                 active_window_bound.top() + active_window_bound.height()-20, "按Esc中止");
            }
            else
            {
                QRect rect = active_window_bound;
                rect.setTopLeft(rect.topLeft() - QPoint(3, 3));
                rect.setBottomRight(rect.bottomRight() + QPoint(3, 3));
                painter.drawRect(rect);
                painter.drawText(active_window_bound.left(),
                                 active_window_bound.top() + active_window_bound.height()+20, "按Esc中止\n请不要移动鼠标");
            }
        }
        else
        {
            painter.drawRect(active_window_bound);
        }
        return;
    }

    painter.fillRect(this->rect(), QColor(0, 0, 0, 0x40)); // 设置透明颜色
    QPen pen;
    pen.setColor(QColor(26, 115, 231, 127));
    painter.setPen(pen);
    painter.drawLine(0, now_point.y(), this->rect().width(), now_point.y());
    painter.drawLine(now_point.x(), 0, now_point.x(), this->rect().height());
    pen.setColor(QColor(123, 123, 233));
    pen.setWidth(2);
    painter.setPen(pen);
    painter.setCompositionMode( QPainter::CompositionMode_Source );
    if(captured->is_begin_draw())
    {
        painter.drawRect(captured->get_x(), captured->get_y(), captured->get_w(), captured->get_h());
        painter.fillRect(captured->get_x(), captured->get_y(), captured->get_w(), captured->get_h(),
                         QColor(0, 0, 0, 0x1));
    }
    QPainterPath path;
    if(is_first_capture && !button_click)
    {
        painter.fillRect(active_window_bound, QColor(0, 0, 0, 0x1));
        painter.drawRect(active_window_bound);
    }
    QList<Capture_region*> list = captured->get_region();
    for(int i=0; i<list.size(); i++)
    {
        QPolygon temp_polygon = list[i]->get_polygon();
        path.addPolygon(temp_polygon);
        path = path.simplified();//防止绘制环形时有一条回到原点的线
    }

    painter.setBrush(QBrush(QColor(0, 0, 0, 1)));
    painter.drawPath(path);
}

void Capture_window::load_key_event(QString name)
{
    if(!Key_manager::is_contains_window(name))
    {
        Key_manager::add_func(name, "leave", [=](bool is_enter){
            if(is_enter)
            {
                if(Config::getConfig<bool>(Config::scroll_capture))
                {
                    end_scroll = true;
                    return;
                }
                Window_manager::pop_window();
            }
        });
        Key_manager::add_func(name, "one_window", [=](bool is_enter){
            if(is_enter)
            {
                for(int i=Config::capture_window_num_begin; i<=Config::capture_window_num_end; i++)
                {
                    Config::setConfig((Config::setting)i, 0);
                }
                Config::setConfig(Config::capture_one_window, 1);
            }
        });
        Key_manager::add_func(name, "multi_window_separate", [=](bool is_enter){
           if(is_enter)
           {
               for(int i=Config::capture_window_num_begin; i<=Config::capture_window_num_end; i++)
               {
                   Config::setConfig((Config::setting)i, 0);
               }
               Config::setConfig(Config::capture_multi_window_separate, 1);
           }
        });
        Key_manager::add_func(name, "multi_window_combine", [=](bool is_enter){
            if(is_enter)
            {
                for(int i=Config::capture_window_num_begin; i<=Config::capture_window_num_end; i++)
                {
                    Config::setConfig((Config::setting)i, 0);
                }
                Config::setConfig(Config::capture_multi_window_combine, 1);
            }
        });
        Key_manager::add_func(name, "save2file", [=](bool is_enter){
            if(is_enter)
            {
                captured->save2file();
            }
        });
        Key_manager::add_func(name, "save2clip", [=](bool is_enter){
            if(is_enter)
            {
                captured->save2clip();
            }
        });
//        Key_manager::add_func(name, "move_all", [=](bool is_enter)
//        {
//            if(captured.isNull())
//            {
//                return;
//            }
//            if(is_enter)
//            {
//                captured->is_key_press(true);
//            }
//            else
//            {
//                captured->is_key_press(false);
//            }
//        });
        Key_manager::add_func(name, "enter_capture", [=](bool is_enter)
        {
            if(is_enter)
            {
                captured->on_click_ok();
            }
        });
    }
}

void Capture_window::mouseMoveEvent(QMouseEvent *event)
{
    now_point = event->pos();
    mouse_move_times++;
    if(Config::getConfig<bool>(Config::scroll_capture))
    {
        return;
    }
    else if(Config::getConfig<bool>(Config::free_capture))
    {
        free_paint_path.lineTo(event->pos());
        update();
        return;
    }
    else if(is_first_capture)
    {
        QRect temp_rect = Window_fliter::instance()->GetWinRectByPoint(event->pos(), false);
        if(temp_rect != active_window_bound)
        {
            active_window_bound = temp_rect;
            update();
        }
    }
    if(!button_click)//设置鼠标样式,必须先要设置mousetracking = true
    {
        if(captured->region_contain(event->globalPos()))
        {
            setCursor(Qt::OpenHandCursor);
        }
        else
        {
            setCursor(Qt::ArrowCursor);
        }
    }
    else
    {
        if(captured->is_press_region())
        {
            setCursor(Qt::ClosedHandCursor);
        }
        captured->mouseMoveEvent(event);
    }
    update();
}

void Capture_window::mousePressEvent(QMouseEvent *event)
{
    if(Config::getConfig<bool>(Config::scroll_capture))
    {
        return;
    }
    if(event->button() == Qt::MidButton)
    {
        captured->on_click_ok();
        return;
    }
    else if(event->button() == Qt::RightButton)
    {
        if(captured->get_region_count() == 0)
        {
            Window_manager::pop_window();
        }
        else
        {
            on_window_cancal();
        }
        update();
        return;
    }
    else if(Config::getConfig<bool>(Config::free_capture))
    {
        button_click = true;
        free_paint_path = QPainterPath();
        free_paint_path.moveTo(event->pos());
        return;
    }
    mouse_move_times = 0;
    button_click = true;
    captured->mousePressEvent(event);
    update();

}

void Capture_window::mouseReleaseEvent(QMouseEvent *event)
{
    if(Config::getConfig<bool>(Config::scroll_capture))
    {
        return;
    }
    else if(Config::getConfig<bool>(Config::free_capture))
    {
        button_click = false;
        hide();
        QScreen * screen = QGuiApplication::primaryScreen();
        QPixmap p = screen->grabWindow(0);
        QPolygon polygon;
        QPixmap mask = QPixmap(p.width(), p.height());
        mask.fill(Qt::transparent);
        QPainter painter(&mask);
        painter.fillPath(free_paint_path.simplified(), QColor(1, 1, 1));
        p.setMask(mask.createMaskFromColor(QColor(1, 1, 1), Qt::MaskOutColor));
        QRect rect = free_paint_path.boundingRect().toRect();
        QPixmap ans = p.copy(rect);
        rect.moveTo(0, 0);
        Window_manager::change_window("Paint_window");
        Window_manager::get_window("Paint_window")->set_pic(ans, rect);
        return;
    }
    QRect rect = QRect(captured->get_x(), captured->get_y(), captured->get_w(), captured->get_h());
    if((rect.width() <= 3 || rect.height() <= 3) && mouse_move_times < 10 && is_first_capture
             && button_click)
    {
        is_first_capture = false;
        captured->combine_region(active_window_bound);
        captured->set_captured_rect(QRect(0, 0, 0, 0));
    }

    button_click = false;
    bool is_begin = captured->is_begin_draw();
    if(rect.width() > 10 && rect.height() > 10)
    {
        is_first_capture = false;
    }
    mouse_move_times = 0;
    captured->mouseReleaseEvent(event);
    if(is_begin)
    {
        update();
    }
}

void Capture_window::on_window_cancal()
{
    button_click = false;
//    is_first_capture = true;
    captured->reset();
    free_paint_path = QPainterPath();
    active_window_bound = QRect();
}

enum window_search_mode {
    INCLUDE_MINIMIZED,
    EXCLUDE_MINIMIZED
};
static bool check_window_valid(HWND window, enum window_search_mode mode)
{
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

static inline HWND next_window(HWND window, enum window_search_mode mode)
{
    while (true) {
        window = GetNextWindow(window, GW_HWNDNEXT);
        if (!window || check_window_valid(window, mode))
            break;
    }

    return window;
}

void Capture_window::on_window_select()
{
//    Window_fliter::instance()->SnapshotAllWinRect();
    /*if(Config::get_config(Config::active_window_capture))
    {
        is_enter = false;
        if(xHook->installMouseHook())
        {
            connect(xHook, &XGlobalHook::mouseEvent, this,
                    [=](XGlobalHook::button_type type, PMOUSEHOOKSTRUCT pMouseHookStruct, bool* is_shield){
                *is_shield = true;
                if(type == XGlobalHook::MOUSE_MOVE)
                {
                    *is_shield = false;
                    POINT point;
                    POINT point2;
                    point2.x = 0;
                    point2.y = 0;
                    GetCursorPos(&point);
                    HWND hwnd = WindowFromPoint(point);
                    RECT rect2;
                    GetClientRect(hwnd, &rect2);
                    ClientToScreen(hwnd, &point2);
                    ScreenToClient(hwnd, &point);
                    if(point.x < 0 || point.y < 0 || point.x > rect2.right || point.y > rect2.bottom)
                    {
                        int title_width = GetSystemMetrics(SM_CYCAPTION);
                        point2.y -=  title_width;
                        rect2.bottom += title_width;
                    }

                    active_window_bound = QRect(QPoint(point2.x, point2.y), QSize(rect2.right, rect2.bottom));
                    update();
                }
                else if(type == XGlobalHook::LBUTTON && !is_enter)
                {
                    is_enter = true;
                    Window_manager::hide_now();
                    QScreen *screen = QGuiApplication::primaryScreen();
                    QPixmap map = screen->grabWindow(0);
                    Window_manager::show_now();
                    POINT point;
                    POINT point2;
                    point2.x = 0;
                    point2.y = 0;
                    GetCursorPos(&point);

                    HWND hwnd = WindowFromPoint(point);
                    RECT rect2;
                    GetClientRect(hwnd, &rect2);
                    ClientToScreen(hwnd, &point2);
                    ScreenToClient(hwnd, &point);
                    if(point.x < 0 || point.y < 0 || point.x > rect2.right || point.y > rect2.bottom)
                    {
                        int title_width = GetSystemMetrics(SM_CYCAPTION);
                        point2.y -=  title_width;
                        rect2.bottom += title_width;
                    }

                    QRect qrect = QRect(QPoint(point2.x, point2.y), QSize(rect2.right, rect2.bottom));
                    QRect temp = qrect;
                    temp.moveTo(0, 0);

                    Window_manager::change_window("Paint_window");
                    Window_manager::get_window("Paint_window")->
                                set_pic(map.copy(qrect), temp);
                    Window_manager::close_window("Capture_window");
                    timer->start(50);//延时卸载钩子，先屏蔽按下事件

                }
                else if(type == XGlobalHook::RBUTTON)
                {
                    Window_manager::pop_window();
                    Window_manager::close_window("Capture_window");
                    timer->start(50);
                }
            });
        }
    }*/
    if(Config::getConfig<bool>(Config::scroll_capture))
    {
        is_enter = false;
        set_scroll_info();
        if(xHook->installMouseHook())
        {
            connect(xHook, &XGlobalHook::mouseEvent, this,
                    [=](XGlobalHook::button_type type, PMOUSEHOOKSTRUCT pMouseHookStruct, bool* is_shield){
                *is_shield = false;
                if(type == XGlobalHook::MOUSE_MOVE && !is_enter)
                {
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

                    if(point.x < 0 || point.y < 0 || point.x > rect2.right || point.y > rect2.bottom)
                    {
                        int title_width = GetSystemMetrics(SM_CYCAPTION);
                        point2.y -=  title_width;
                        rect2.bottom += title_width;
                    }
                    active_window_bound = QRect(QPoint(point2.x, point2.y), QSize(rect2.right, rect2.bottom));
                    update();
                }
                else if(type == XGlobalHook::LBUTTON && !is_enter)
                {
                    end_scroll = false;
                    is_enter = true;
                    *is_shield = true;
                    POINT point;
                    POINT point2;
                    point2.x = 0;
                    point2.y = 0;
                    GetCursorPos(&point);
                    cursor_point.setX(point.x);
                    cursor_point.setY(point.y);
                    scroll_hwnd = WindowFromPoint(point);
                    QScreen * screen = QGuiApplication::primaryScreen();
                    QPixmap pix = screen->grabWindow(WId(scroll_hwnd));
                    QImage image = pix.toImage();
//                    image.save("f:/dinfo/temp.png");
                    for(int i=0; i<image.height(); i++)
                    {
                        for(int k=0; k<image.width(); k++)
                        {
                            if(image.pixel(k, i) != 0xff000000)
                            {
                                window_valid = true;
                                goto WINDOW_VALID_OUT;
                            }
                        }
                    }
                    window_valid = false;

WINDOW_VALID_OUT:;
                    update();
                    int time = Config::getConfig<int>(Config::capture_interval);
//                    if(!window_valid)
//                    {
//                        time = 300;
//                    }
                    scroll_timer->start(time);
                    time = QDateTime::currentMSecsSinceEpoch();
                }
                else if(type == XGlobalHook::RBUTTON && !is_enter)
                {
                    *is_shield = true;
                    Window_manager::pop_window();
                    timer->start(50);
                }
            });
        }
    }
}

void Capture_window::set_scroll_info()
{
    if(Config::getConfig<bool>(Config::scroll_capture))
    {
        dispatcher = new Scroll_dispatcher(this);
        connect(dispatcher, &Scroll_dispatcher::finish, this, [=](QImage image){
            Window_manager::change_window("Paint_window");
            QPixmap pixmap;
            pixmap.convertFromImage(image);
            Window_manager::get_window("Paint_window")->
                    set_pic(pixmap, QRect(0, 0, image.width(), image.height()));
            timer->start(50);

        });
        scroll_timer = new QTimer();
        connect(scroll_timer, &QTimer::timeout, this, [=](){
            QScreen * screen = QGuiApplication::primaryScreen();
            QPixmap pix;
            if(!window_valid)
            {
                pix = screen->grabWindow(0, active_window_bound.x(), active_window_bound.y(),
                                                 active_window_bound.width(), active_window_bound.height());
            }
            else
            {
                pix = screen->grabWindow(WId(scroll_hwnd));
            }

            QWindow* mainWindow = QWindow::fromWinId(WId(scroll_hwnd));
            QPoint window_point;
            if (mainWindow != nullptr)
            {
               window_point = mainWindow->framePosition();
            }
            if(false)
            {
                INPUT inputs[1] = {};
                ZeroMemory(inputs, sizeof(inputs));
                inputs[0].type = INPUT_MOUSE;
                inputs[0].mi.mouseData = -240;
                inputs[0].mi.dwFlags = MOUSEEVENTF_WHEEL;
                inputs[0].mi.dx = 0;
                inputs[0].mi.dy = 0;
                inputs[0].mi.time = 0;
                SendInput(ARRAYSIZE(inputs), inputs, sizeof(INPUT));
            }
            else
            {
                PostMessage(scroll_hwnd, WM_MOUSEWHEEL, MAKEWPARAM(0, -240), MAKELPARAM(cursor_point.x(),
                                                                                   cursor_point.y()));
            }
            QImage image = pix.toImage();

            bool success = false;
            cv::Mat mat1 = Image_helper::QImage2Mat(pre_image);
            if(mat1.cols != 0)
            {
                cv::Mat mat2 = Image_helper::QImage2Mat(image);
                if(!Image_helper::is_equal(mat1, mat2))success = true;
            }
            else
            {
                success = true;
            }
            if(!success || end_scroll)
            {
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

