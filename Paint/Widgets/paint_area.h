#ifndef PAINT_AREA_H
#define PAINT_AREA_H
#include<QScrollArea>

class Paint_area : public QScrollArea
{
public:
    Paint_area();
    Paint_area(QWidget* parent=nullptr);
};

#endif // PAINT_AREA_H
