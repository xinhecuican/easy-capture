#ifndef CAPTURE_REGION_H
#define CAPTURE_REGION_H

#include<QPolygon>
#include "stretch_point.h"
#include "Helper/plist.h"
#include<QDebug>
#include<stdexcept>
#include<iostream>
#include "Ipoint_position_change.h"

class Capture_region
{
public:
    Capture_region();
    Capture_region(QWidget* parent);
    Capture_region(Ipoint_position_change* region, QWidget* parent);
    Capture_region(Ipoint_position_change* region, QWidget* parent, QRect rect);
    ~Capture_region();
    void unit(QRect &rect);
    void unit(Capture_region &region);
    bool intersected(Capture_region &region);
    bool intersected(QRect &rect);
    void move(int dx, int dy);
    void point_move(QList<int> position, int x, int y);
    bool contains(QPoint point);
    void clear();
    inline int get_size(){return points.size();}
    void on_pos_change(int pos);//当在capture_area中的位置改变时触发
    QPolygon get_polygon();
private:
    QWidget* parent;
    Ipoint_position_change* region;
    QPolygon polygon;
    PList<Stretch_point*> points;
    PList<Stretch_point*> free_points;
    void set_nodes();
    void create_point(int index);

};

#endif // CAPTURE_REGION_H

