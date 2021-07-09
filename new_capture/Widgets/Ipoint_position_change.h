#ifndef IPOINT_POSITION_CHANGE_H
#define IPOINT_POSITION_CHANGE_H
#include<QList>
class Ipoint_position_change
{
public:
    virtual void control_point_position_change(int index, QList<int> position, int dx, int dy) = 0;
    virtual void position_change_begin(int index, QPoint position){};
    virtual void position_change_end(QPoint position){};
};

#endif // IPOINT_POSITION_CHANGE_H
