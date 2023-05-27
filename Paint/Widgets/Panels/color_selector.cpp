#include "color_selector.h"
#include<QPainter>
#include<QGuiApplication>
#include<QScreen>
#include<QApplication>
#include<QDesktopWidget>
#include<QRgb>
#include<QColor>
#include<QDebug>
#include<QTimer>
#include "Helper/imagehelper.h"

Color_selector::Color_selector() {

}

Color_selector::~Color_selector() {

}

Color_selector::Color_selector(QWidget* parent) : QMainWindow(parent) {
    setWindowFlag(Qt::FramelessWindowHint);
    setAttribute(Qt::WA_TranslucentBackground);
    setAttribute(Qt::WA_DeleteOnClose, true);
    showFullScreen();
    this->setMouseTracking(true);
    setCursor(QCursor(QPixmap(":/image/color_dropper.png"), 0, 32));
    window_w = QApplication::desktop()->width();
    window_h = QApplication::desktop()->height();
    QTimer* timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, [=]() {
        region = ImageHelper::grabScreen(pos.x()-5, pos.y()-3, 11, 7).toImage();
        selected_color = cal_dest_color(region.pixel(5, 3));
        decide_region_position(pos);
        update();
    });
    timer->start(10);
}

void Color_selector::paintEvent(QPaintEvent *event) {
    if(region.width() < 11) {
        return;
    }
    QPainter painter(this);
    painter.fillRect(this->rect(), src); // 设置透明颜色
    QImage image = QImage(110, 70,QImage::Format_RGB32);
    for(int i=0; i<11; i++) {
        for(int k=0; k<7; k++) {
            for(int j=i*10; j<i*10+10; j++) {
                for(int m=k*10; m<k*10+10; m++) {
                    image.setPixel(j, m, region.pixel(i, k));
                }
            }
        }
    }
    QImage image2 = QImage(10, 10, QImage::Format_RGB32);
    image2.fill(selected_color);
    painter.drawImage(region_pos.x(), region_pos.y(), image);
    painter.drawImage(region_pos.x(), region_pos.y()+70, image2);
    painter.drawText(region_pos.x()+10, region_pos.y()+80, "rgb:"+QString::number(selected_color.red())
                     + "," + QString::number(selected_color.green()) + "," + QString::number(selected_color.blue()));
    painter.setCompositionMode(QPainter::RasterOp_SourceOrNotDestination);
    painter.drawLine(region_pos.x()+50, region_pos.y()+35, region_pos.x()+60, region_pos.y()+35);
    painter.drawLine(region_pos.x()+55, region_pos.y()+30, region_pos.x()+55, region_pos.y()+40);
}

void Color_selector::mousePressEvent(QMouseEvent *event) {
    hide();
    region = ImageHelper::grabScreen(event->pos().x()-5, event->pos().y()-3, 11, 7).toImage();
    selected_color = region.pixel(5, 3);
    if(event->button() == Qt::LeftButton) {
        emit color_select(selected_color);
    }
    this->close();
}

void Color_selector::mouseMoveEvent(QMouseEvent *event) {
    pos = event->pos();
}

void Color_selector::decide_region_position(QPoint mouse_pos) {
    region_pos.setX(mouse_pos.x()+32);
    region_pos.setY(mouse_pos.y()-32-80);
    if(mouse_pos.x()+32+110 > window_w) {
        region_pos.setX(window_w - 110-32);
    }
    if(mouse_pos.x()+32 <= 0) {
        region_pos.setX(0);
    }
    if(mouse_pos.y()-32-80 <= 0) {
        region_pos.setY(32+70);
    }
    if(mouse_pos.y()-32-80 >= window_h) {
        region_pos.setY(window_h - 80);
    }
}

QColor Color_selector::cal_dest_color(QColor res) {
    int ra = res.alpha();
    int rr = res.red();
    int rg = res.green();
    int rb = res.blue();

    int sa = src.alpha();
    int pre_alpha = 255 - sa;
    int desta = (ra-sa) * 255 / pre_alpha;
    int destr = rr * 255 / pre_alpha;
    int destg = rg * 255 / pre_alpha;
    int destb = rb * 255 / pre_alpha;
    return QColor(destr, destg, destb, desta);
}
