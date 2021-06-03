#include "capture_area.h"
#include<QDebug>
#include<QSizePolicy>
#include<math.h>

Capture_area::Capture_area(QWidget* parent):QWidget(parent)
{
    setFixedSize(0, 0);
    begin_point = QPoint(0, 0);
    end_point = QPoint(0, 0);
}

void Capture_area::mouseMoveEvent(QMouseEvent *event)
{
    end_point = event->globalPos();
    int min_x, min_y, height, weight;
    if(end_point.x() > begin_point.x())
    {
        min_x = begin_point.x();
        weight = end_point.x() - begin_point.x();
    }
    else
    {
        min_x = end_point.x();
        weight = begin_point.x() - end_point.x();
    }

    if(end_point.y() > begin_point.y())
    {
        min_y = begin_point.y();
        height = end_point.y() - begin_point.y();
    }
    else
    {
        min_y = end_point.y();
        height = begin_point.y() - end_point.y();
    }
    move(min_x, min_y);

    setFixedSize(weight, height);
    update();
}

void Capture_area::mousePressEvent(QMouseEvent *event)
{
    begin_point = event->globalPos();
}


void Capture_area::mouseReleaseEvent(QMouseEvent *event)
{
}

void Capture_area::paintEvent(QPaintEvent *event)
{
    setStyleSheet(QString::fromUtf8("border:1px soild red"));
}

int Capture_area::get_x()
{
    return begin_point.x() > end_point.x() ? end_point.x() : begin_point.x();
}

int Capture_area::get_y()
{
    return begin_point.y() > end_point.y() ? end_point.y() : begin_point.y();
}

int Capture_area::get_w()
{
    return abs(begin_point.x() - end_point.x());
}

int Capture_area::get_h()
{
    return abs(begin_point.y() - end_point.y());
}

