#ifndef IPOINT_POSITION_CHANGE_H
#define IPOINT_POSITION_CHANGE_H
#include<QList>
class Ipoint_position_change
{
public:
    virtual void control_point_position_change(int index, QList<int> position, int dx, int dy) = 0;
};

#endif // IPOINT_POSITION_CHANGE_H
