#include "capture_area.h"
#include<QDebug>
#include<QSizePolicy>
#include<math.h>
#include<QPainter>
#include<QDesktopWidget>
#include<QApplication>
#include "config.h"

Capture_area::Capture_area(QWidget* parent):QWidget(parent)
{
    setFixedSize(0, 0);
    begin_point = QPoint(0, 0);
    end_point = QPoint(0, 0);
    regions = QList<QRegion>();

    begin_draw = false;
    setAttribute(Qt::WA_TransparentForMouseEvents, false);
    setMouseTracking(true);
}

Capture_area::~Capture_area()
{

}

void Capture_area::mouseMoveEvent(QMouseEvent *event)
{
    event->accept();
    if(begin_draw)
    {
        end_point = event->globalPos();
    }
    else
    {
        end_point = event->globalPos();
        int w = end_point.x() - begin_point.x();
        int h = end_point.y() - begin_point.y();
        begin_point = end_point;
        QRegion temp_region ;
        for(int i=0; i<regions.count(); i++)
        {
            if(regions[i].contains(QPoint(event->globalX(), event->globalY())))
            {
                temp_region = regions[i].translated(w, h);
                regions.removeAt(i);
                break;
            }
        }
        combine_region(temp_region);
    }
}

void Capture_area::mousePressEvent(QMouseEvent *event)
{
    event->accept();
    if(!region_contain(event->globalPos()))
    {
        begin_draw = true;
        begin_point = event->globalPos();
        end_point = begin_point;
    }
    else
    {
        begin_point = event->globalPos();
        end_point = begin_point;
    }
}


void Capture_area::mouseReleaseEvent(QMouseEvent *event)
{
    if(begin_draw)
    {
        begin_draw = false;
        QRect rect = QRect(get_x(), get_y(), get_w(), get_h());
        combine_region(rect);
    }
}

void Capture_area::paintEvent(QPaintEvent *event)
{

}

void Capture_area::reset()
{
    begin_point.setX(0);
    begin_point.setY(0);
    end_point.setX(0);
    end_point.setY(0);
    regions.clear();
}

void Capture_area::combine_region(QRect rect)
{
    QList<QRegion> temp;
    QList<QRegion>::iterator iter = regions.begin();
    while(iter != regions.end())
    {
        if(!iter->intersected(rect).isNull())
        {
            temp.append(*iter);
            iter = regions.erase(iter);
        }
        else
        {
            ++iter;
        }
    }

    QRegion temp_region = QRegion(rect);
    if(!temp.isEmpty())
    {
        foreach(QRegion region, temp)
        {
            temp_region = temp_region.united(region);
        }
    }
    regions.append(temp_region);
}

void Capture_area::combine_region(QRegion region)
{
    QList<QRegion> temp;
    QList<QRegion>::iterator iter = regions.begin();
    while(iter != regions.end())
    {
        if(!iter->intersected(region).isNull())
        {
            temp.append(*iter);
            iter = regions.erase(iter);
        }
        else
        {
            ++iter;
        }
    }

    QRegion temp_region = QRegion(region);
    if(!temp.isEmpty())
    {
        foreach(QRegion region, temp)
        {
            temp_region = temp_region.united(region);
        }
    }
    regions.append(temp_region);
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
    return abs(end_point.x() - begin_point.x());
}

int Capture_area::get_h()
{
    return abs(end_point.y() - begin_point.y());
}

bool Capture_area::is_begin_draw()
{
    return begin_draw;
}

QList<QRegion> Capture_area::get_region()
{
    return regions;
}

bool Capture_area::region_contain(QPoint p)
{
    foreach(QRegion region, regions)
    {
        if(region.contains(p))
        {
            return true;
        }
    }
    return false;
}

