#ifndef CAPTURE_AREA_H
#define CAPTURE_AREA_H
#include<QWidget>
#include<QDebug>
#include<QMouseEvent>
#include<QPoint>

class Capture_area : QWidget
{
public:
    Capture_area(QWidget* parent);
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void paintEvent(QPaintEvent *event) override;

    int get_x();
    int get_y();
    int get_w();
    int get_h();

    QPoint begin_point;
    QPoint end_point;
private:

};

#endif // CAPTURE_AREA_H
