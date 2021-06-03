#include "capture_window.h"
#include "ui_capture_window.h"
#include "window_manager.h"
#include<QDebug>
#include<QPainter>

Capture_window::Capture_window(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::Capture_window)
{
    ui->setupUi(this);
    this->points = QVector<QPoint>();
    this->captured = new Capture_area(this);



    Window_manager::push_window("capture", this);
    setWindowFlag(Qt::FramelessWindowHint);
    setAttribute(Qt::WA_TranslucentBackground);
    showFullScreen();
}

Capture_window::~Capture_window()
{
    delete ui;
    delete captured;
}

void Capture_window::paintEvent(QPaintEvent *paint_event)
{
    QPainter painter(this);

    painter.fillRect(this->rect(), QColor(0, 0, 0, 0x20)); /* 设置透明颜色 */
    painter.setCompositionMode( QPainter::CompositionMode_Clear );
    painter.fillRect(captured->get_x(), captured->get_y(), captured->get_w(), captured->get_h(),
                     QColor(0, 0, 0, 0));
    qDebug() << captured->get_x() ;
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
    captured->mouseMoveEvent(event);
    update();
}

void Capture_window::mousePressEvent(QMouseEvent *event)
{
    captured->mousePressEvent(event);
    update();
}

void Capture_window::mouseReleaseEvent(QMouseEvent *event)
{

}

