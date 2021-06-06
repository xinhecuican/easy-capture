#include "capture_window.h"
#include "ui_capture_window.h"
#include "window_manager.h"
#include<QDebug>
#include<QPainter>
#include<QPen>
#include<QDesktopWidget>


Capture_window::Capture_window(QWidget *parent) :
    Window_base(parent),
    ui(new Ui::Capture_window)
{
    ui->setupUi(this);
    this->points = QVector<QPoint>();
    this->captured = new Capture_area(this);
    button_click = false;

    QDesktopWidget* desktop = QApplication::desktop();
    total_region = QRegion(desktop->screenGeometry());

    Window_manager::push_window("capture", this);
    setWindowFlag(Qt::FramelessWindowHint);
    setAttribute(Qt::WA_TranslucentBackground);
    showFullScreen();
    this->setMouseTracking(true);
    this->ui->centralwidget->setMouseTracking(true);
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
    QPen pen(QColor(0, 0, 0, 0));
    painter.setPen(pen);
    foreach(QRegion region, captured->get_region())
    {
        path.addRegion(region);
    }
    painter.setBrush(QBrush(QColor(0, 0, 0, 1)));
    painter.drawPath(path);
}

void Capture_window::keyPressEvent(QKeyEvent *event)
{
    if(event->key() == Qt::Key_Escape)
    {
        Window_manager::pop_window();
    }
}

void Capture_window::mouseMoveEvent(QMouseEvent *event)
{
    if(!button_click)//设置鼠标样式，必须要在ui中设置mous
    {
        qDebug() << event->globalPos();
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
        setCursor(Qt::ClosedHandCursor);
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

}

void Capture_window::on_window_select()
{

}

