#ifndef CAPTURE_AREA_H
#define CAPTURE_AREA_H
#include<QWidget>
#include<QDebug>
#include<QMouseEvent>
#include<QPoint>
#include<QRegion>
#include<QList>
#include<QDialogButtonBox>
#include"stretch_point.h"
#include "Helper/plist.h"
#include "Ipoint_position_change.h"
#include "capture_region.h"

class Capture_area : public QWidget, Ipoint_position_change
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
    QRect bounded_rect();
    void add_region(Capture_region* region);
    void on_region_remove(Capture_region* region);
    void control_point_position_change(int index, QList<int> position, int dx, int dy) override;

    int get_x();
    int get_y();
    int get_w();
    int get_h();
    void set_captured_rect(QRect rect);
    QList<Capture_region*>& get_region();
    int get_region_count();
    bool is_begin_draw();
    bool is_press_region();
    void cal_button_pos();
    void combine_region(QRect rect);
    void is_key_press(bool enter);

    QPoint begin_point;
    QPoint end_point;


public slots:
    void on_click_ok();
private:
    bool key_press;
    //QList<QRegion> regions;
    bool begin_draw;
    QRect all_rect;

    void combine_region(Capture_region* region);

    bool _is_press_region;
    QWidget* parent;
    int now_index = 0;

    PList<Capture_region*> regions;

    QDialogButtonBox* button_box;
    QPushButton* ok;
    QPushButton* cancel;

};

#endif // CAPTURE_AREA_H
