#ifndef PICTURE_LAYER_H
#define PICTURE_LAYER_H
#include<QWidget>
#include<QPixmap>
#include<QPolygon>
#include<QPainter>
#include "new_capture/Widgets/Ipoint_position_change.h"
#include "new_capture/Widgets/capture_region.h"
#include "Helper/plist.h"

class Picture_layer : public QWidget, Ipoint_position_change
{
public:
    Picture_layer();
    Picture_layer(QPixmap picture, QRect rect, QWidget* parent=nullptr);
    ~Picture_layer();
    void paint(QImage& image);
    QPixmap* get_pic();
    void control_point_position_change(int index, QList<int> position, int dx, int dy) override;
private:
    QWidget* parent;
    QPixmap picture;
    Capture_region* region;
    QRect base_rect;
};

#endif // PICTURE_LAYER_H
