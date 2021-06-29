#include "capture_window.h"
#include "ui_capture_window.h"
#include "Manager/window_manager.h"
#include<QDebug>
#include<QPainter>
#include<QPen>
#include<QDesktopWidget>
#include "Manager/config.h"
#include "Manager/key_manager.h"


Capture_window::Capture_window(QWidget *parent) :
    Window_base(parent, this, "Capture_window"),
    ui(new Ui::Capture_window)
{
    ui->setupUi(this);
    button_click = false;

    QDesktopWidget* desktop = QApplication::desktop();
    total_region = QRegion(desktop->screenGeometry());

    setWindowFlag(Qt::FramelessWindowHint);
    setAttribute(Qt::WA_TranslucentBackground);
    showFullScreen();
    this->setMouseTracking(true);
    this->ui->centralwidget->setMouseTracking(true);
    this->captured = new Capture_area(this);

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
    QList<Capture_region> list = captured->get_region();
    for(int i=0; i<list.size(); i++)
    {
        QPolygon temp_polygon = list[i].get_polygon();
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
    button_click = true;
    captured->mousePressEvent(event);
    update();

}

void Capture_window::mouseReleaseEvent(QMouseEvent *event)
{
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
}

void Capture_window::on_window_select()
{

}

