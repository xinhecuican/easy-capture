#include "capture_window.h"
#include "ui_capture_window.h"
#include "Manager/window_manager.h"
#include<QDebug>
#include<QPainter>
#include<QPen>
#include<QDesktopWidget>
#include "Manager/config.h"
#include "Manager/key_manager.h"
#include "windows.h"
#include<QScreen>
#include "hook.h"

Capture_window::Capture_window(QWidget *parent) :
    Window_base(parent, this, "Capture_window"),
    ui(new Ui::Capture_window)
{
    ui->setupUi(this);
    button_click = false;

    setWindowFlag(Qt::FramelessWindowHint);
    setAttribute(Qt::WA_TranslucentBackground);
    showFullScreen();
    this->setMouseTracking(true);
    this->ui->centralwidget->setMouseTracking(true);
    this->captured = new Capture_area(this);
    setWindowFlags(windowFlags() | Qt::WindowStaysOnTopHint);

    load_key_event("Capture_window");
}

Capture_window::~Capture_window()
{
    delete ui;
    delete captured;
}

void Capture_window::paintEvent(QPaintEvent *paint_event)
{
    QPainter painter(this);

    if(Config::get_config(Config::free_capture) && button_click)
    {
        QPen pen;
        pen.setStyle(Qt::DashLine);
        pen.setColor(QColor(123, 123, 233));
        pen.setWidth(3);
        painter.setPen(pen);
        painter.drawPath(free_paint_path);
        return;
    }
    else if(Config::get_config(Config::active_window_capture))//活动窗口截屏
    {
        QPen pen;
        pen.setColor(QColor(255, 0, 0));
        pen.setWidth(3);
        painter.setPen(pen);
        painter.drawRect(active_window_bound);
        return;
    }

    painter.fillRect(this->rect(), QColor(0, 0, 0, 0x20)); // 设置透明颜色
    painter.setCompositionMode( QPainter::CompositionMode_Source );
    if(captured->is_begin_draw())
    {
        painter.fillRect(captured->get_x(), captured->get_y(), captured->get_w(), captured->get_h(),
                         QColor(0, 0, 0, 0x1));
    }
    QPainterPath path;
    QPen pen(QColor(123, 123, 233));
    painter.setPen(pen);
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
                Window_manager::pop_window();
            }
        });
        Key_manager::add_func(name, "one_window", [=](bool is_enter){
            if(is_enter)
            {
                for(int i=Config::capture_window_num_begin; i<=Config::capture_window_num_end; i++)
                {
                    Config::set_config((Config::setting)i, 0);
                }
                Config::set_config(Config::capture_one_window, 1);
            }
        });
        Key_manager::add_func(name, "multi_window_separate", [=](bool is_enter){
           if(is_enter)
           {
               for(int i=Config::capture_window_num_begin; i<=Config::capture_window_num_end; i++)
               {
                   Config::set_config((Config::setting)i, 0);
               }
               Config::set_config(Config::capture_multi_window_separate, 1);
           }
        });
        Key_manager::add_func(name, "multi_window_combine", [=](bool is_enter){
            if(is_enter)
            {
                for(int i=Config::capture_window_num_begin; i<=Config::capture_window_num_end; i++)
                {
                    Config::set_config((Config::setting)i, 0);
                }
                Config::set_config(Config::capture_multi_window_combine, 1);
            }
        });
        Key_manager::add_func(name, "move_all", [=](bool is_enter)
        {
            if(is_enter)
            {
                captured->key_press = true;
            }
            else
                captured->key_press = false;
        });
    }
}
/*
void Capture_window::keyPressEvent(QKeyEvent *event)
{
    if(event->key() == Qt::Key_Escape)
    {
        Window_manager::pop_window();
    }
}
*/
void Capture_window::mouseMoveEvent(QMouseEvent *event)
{
    if(Config::get_config(Config::free_capture))
    {
        free_paint_path.lineTo(event->pos());
        update();
        return;
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
    if(event->button() == Qt::MidButton)
    {
        captured->on_click_ok();
        return;
    }
    else if(event->button() == Qt::RightButton)
    {
        return;
    }
    else if(Config::get_config(Config::free_capture))
    {
        button_click = true;
        free_paint_path = QPainterPath();
        free_paint_path.moveTo(event->pos());
        return;
    }
    button_click = true;
    captured->mousePressEvent(event);
    update();

}

void Capture_window::mouseReleaseEvent(QMouseEvent *event)
{
    if(Config::get_config(Config::free_capture))
    {
        button_click = false;
        Config::set_config(Config::free_capture, false);
        hide();
        QScreen * screen = QGuiApplication::primaryScreen();
        QPixmap p = screen->grabWindow(0);
        QPixmap temp = p.copy();
        temp.fill(Qt::transparent);
        //p.fill(Qt::transparent);
        QPainter painter(&temp);
        painter.setCompositionMode(QPainter::CompositionMode_Source);
        painter.drawPixmap(0, 0, p);
        painter.setCompositionMode( QPainter::CompositionMode_DestinationIn );
        painter.setBackgroundMode(Qt::OpaqueMode);
        QPainterPath temp_path = QPainterPath();
        temp_path.addRect(p.rect());
        temp_path = temp_path.subtracted(free_paint_path.simplified());
        painter.fillPath(temp_path, QColor(0, 0, 0, 0));
        QRect rect = free_paint_path.boundingRect().toRect();
        QPixmap temp_copy = temp.copy(rect);
        rect.moveTo(0, 0);
        Window_manager::change_window("Paint_window");
        Window_manager::get_window("Paint_window")->
                set_pic(temp_copy, rect);

        return;
    }
    button_click = false;
    bool is_begin = captured->is_begin_draw();
    captured->mouseReleaseEvent(event);
    if(is_begin)
    {
        update();
    }
}

void Capture_window::on_window_cancal()
{
    button_click = false;
    captured->reset();
    free_paint_path = QPainterPath();
}

void Capture_window::on_window_select()
{
    if(Config::get_config(Config::active_window_capture))
    {
        is_enter = false;
        if(xHook->installMouseHook())
        {
            connect(xHook, &XGlobalHook::mouseEvent, this,
                    [=](XGlobalHook::button_type type, PMOUSEHOOKSTRUCT pMouseHookStruct){
                if(type == XGlobalHook::MOUSE_MOVE)
                {
                    QScreen *screen = QGuiApplication::primaryScreen();
                    QPixmap map = screen->grabWindow(0);
                    POINT point;
                    POINT point2;
                    point2.x = 0;
                    point2.y = 0;
                    GetCursorPos(&point);
                    HWND hwnd = WindowFromPoint(point);
                    RECT rect2;
                    GetClientRect(hwnd, &rect2);
                    ClientToScreen(hwnd, &point2);

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

                    QRect qrect = QRect(QPoint(point2.x, point2.y), QSize(rect2.right, rect2.bottom));
                    QRect temp = qrect;
                    temp.moveTo(0, 0);

                    Window_manager::change_window("Paint_window");
                    Window_manager::get_window("Paint_window")->
                                set_pic(map.copy(qrect), temp);
                    Window_manager::close_window("Capture_window");
                    close();
                }
                else if(type == XGlobalHook::LBUTTON_UP)
                {
                    xHook->uninstallMouseHook();
                }
                else if(type == XGlobalHook::RBUTTON)
                {
                    xHook->uninstallMouseHook();
                    Window_manager::pop_window();
                }
            });
        }
    }
}

