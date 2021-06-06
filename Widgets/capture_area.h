#ifndef CAPTURE_AREA_H
#define CAPTURE_AREA_H
#include<QWidget>
#include<QDebug>
#include<QMouseEvent>
#include<QPoint>
#include<QRegion>
#include<Helper/my_math.h>
#include<QList>

class Capture_area : QWidget
{
public:
    Capture_area(QWidget* parent);
    ~Capture_area();
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void paintEvent(QPaintEvent *event) override;
    void reset();
    bool region_contain(QPoint p);

    int get_x();
    int get_y();
    int get_w();
    int get_h();
    QList<QRegion> get_region();
    bool is_begin_draw();

    QPoint begin_point;
    QPoint end_point;
private:
    QList<QRegion> regions;
    bool begin_draw;
    QRect all_rect;
    void combine_region(QRect rect);
    void combine_region(QRegion region);


};

#endif // CAPTURE_AREA_H
