#include "capture_area.h"
#include<QDebug>
#include<QSizePolicy>
#include<math.h>
#include<QPainter>
#include<QDesktopWidget>
#include<QApplication>
#include "Manager/config.h"

Capture_area::Capture_area(QWidget* parent):QWidget(parent)
{
    this->parent = parent;
    setFixedSize(0, 0);
    begin_point = QPoint(0, 0);
    end_point = QPoint(0, 0);
    regions = QList<Capture_region>();
    key_press = false;
    begin_draw = false;

    setAttribute(Qt::WA_TransparentForMouseEvents, false);
    setMouseTracking(true);
}

void Capture_area::reset()
{
    begin_point.setX(0);
    begin_point.setY(0);
    end_point.setX(0);
    end_point.setY(0);
    for(int i=0; i<regions.size(); i++)
    {
        regions[i].clear();
    }
    regions.clear();
    begin_draw = false;
    key_press = false;
    _is_press_region = false;
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
    else if(key_press || (Config::get_config(Config::capture_one_window) && _is_press_region))//按下ctrl键全部移动
    {
        end_point = event->globalPos();
        int w = end_point.x() - begin_point.x();
        int h = end_point.y() - begin_point.y();
        begin_point = end_point;
        for(int i=0; i<regions.count(); i++)
        {
            regions[i].move(w, h);
        }
    }
    else
    {

        end_point = event->globalPos();
        int w = end_point.x() - begin_point.x();
        int h = end_point.y() - begin_point.y();
        begin_point = end_point;
        if(Config::get_config(Config::capture_multi_window_combine))
        {
            Capture_region temp_region ;
            for(int i=0; i<regions.count(); i++)
            {
                if(regions[i].contains(QPoint(event->globalX(), event->globalY())))
                {
                    regions[i].move(w, h);
                    temp_region = regions[i];
                    regions.removeAt(i);
                    break;
                }
            }
            combine_region(temp_region);
        }
        else if(Config::get_config(Config::capture_multi_window_separate))
        {
            regions[now_index].move(w, h);
        }
    }
}

void Capture_area::mousePressEvent(QMouseEvent *event)
{
    event->accept();
    if(!region_contain(event->globalPos()))
    {
        _is_press_region = false;
        if(Config::get_config(Config::capture_one_window) && regions.count() >= 1)
        {
            begin_draw = false;
        }
        else
        {
            begin_draw = true;
        }
        begin_point = event->globalPos();
        end_point = begin_point;
    }
    else
    {
        _is_press_region = true;
        begin_point = event->globalPos();
        end_point = begin_point;
        if(Config::get_config(Config::capture_multi_window_separate))
        {
            for(int i=0; i<regions.count(); i++)
            {
                if(regions[i].contains(event->globalPos()))
                {
                    now_index = i;
                }
            }
        }

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

void Capture_area::add_region(Capture_region &region)
{
    regions.append(region);
}

void Capture_area::on_region_remove(Capture_region &region)
{
    region.clear();
}


void Capture_area::combine_region(QRect rect)
{
    QList<Capture_region> temp;
    QList<Capture_region>::iterator iter = regions.begin();
    while(iter != regions.end())
    {
        if(iter->intersected(rect))
        {
            temp.append(*iter);
            on_region_remove(*iter);
            iter = regions.erase(iter);
        }
        else
        {
            ++iter;
        }
    }

    Capture_region temp_region = Capture_region(this, parent, rect);
    if(!temp.isEmpty())
    {
        foreach(Capture_region region, temp)
        {
            temp_region.unit(region);
        }
    }
    add_region(temp_region);
    for(int i=0; i<regions.size(); i++)
    {
        regions[i].on_pos_change(i);
    }
}

void Capture_area::combine_region(Capture_region region)
{
    QList<Capture_region> temp;
    QList<Capture_region>::iterator iter = regions.begin();
    while(iter != regions.end())
    {
        if(iter->intersected(region))
        {
            temp.append(*iter);
            on_region_remove(*iter);
            iter = regions.erase(iter);
        }
        else
        {
            ++iter;
        }
    }

    if(!temp.isEmpty())
    {
        foreach(Capture_region cregion, temp)
        {
            region.unit(cregion);
        }
    }
    add_region(region);
}

void Capture_area::keyPressEvent(QKeyEvent *event)
{
    if(event->key() == Qt::Key_Control)
    {
        key_press = true;
    }
}

void Capture_area::keyReleaseEvent(QKeyEvent *event)
{
    if(event->key() == Qt::Key_Control)
    {
        key_press = false;
    }
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

int Capture_area::get_region_count()
{
    return regions.count();
}

bool Capture_area::is_begin_draw()
{
    return begin_draw;
}

bool Capture_area::is_press_region()
{
    return _is_press_region;
}

QList<Capture_area::Capture_region> Capture_area::get_region()
{
    return regions;
}

bool Capture_area::region_contain(QPoint p)
{
    foreach(Capture_region region, regions)
    {
        if(region.contains(p))
        {
            return true;
        }
    }
    return false;
}

void Capture_area::control_point_position_change(int index, QList<int> position, int dx, int dy)
{
    regions[index].point_move(position, dx, dy);
}
