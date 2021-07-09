#ifndef PICTURE_LAYER_H
#define PICTURE_LAYER_H
#include<QWidget>
#include<QPixmap>
#include<QPolygon>
#include<QPainter>
#include "new_capture/Widgets/Ipoint_position_change.h"
#include "new_capture/Widgets/capture_region.h"
#include "Helper/plist.h"
#include "Paint/Data/Ilayer.h"
#include "paint_layer.h"
#include "Base/Record_element.h"

class Picture_layer : public QWidget, Ipoint_position_change, public Ilayer
{
public:
    Picture_layer();
    Picture_layer(QString name, QPixmap picture, QRect rect, QWidget* parent=nullptr);
    ~Picture_layer();
    void paint(QImage& image)override;
    void erase_and_paint(QPoint point, QImage &image) override;
    void set_name(QString name) override;
    int add_data(Paint_data *style, QPainterPath path) override;
    QPixmap* get_pic();
    QRect bounded_rect()override;
    void control_point_position_change(int index, QList<int> position, int dx, int dy) override;
    void position_change_begin(int index, QPoint position) override;
    void position_change_end(QPoint position) override;
    bool redo(int index = 0) override;
    bool undo(int index = 0) override;
    QString get_name() override;

private:
    struct node
    {
        int dx;
        int dy;
        int index;
    };
    QList<node> pos_change_list;
    int now_pos;
    QString name;
    QWidget* parent;
    QPixmap picture;
    Capture_region* region;
    QRect base_rect;
    Paint_layer* paint_layer;
};

#endif // PICTURE_LAYER_H
