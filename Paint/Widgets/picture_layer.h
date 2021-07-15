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

class Picture_layer : public QWidget, public Ilayer
{
public:
    enum DragPos{top_left, top_right, down_left, down_right};
    Picture_layer();
    Picture_layer(QString name, QPixmap picture, QRect rect, QWidget* parent=nullptr);
    ~Picture_layer();
    void paint(QImage& image)override;
    void erase_and_paint(QPoint point, QImage &image) override;
    void set_name(QString name) override;
    int add_data(Paint_data *style, QPainterPath path) override;
    QPixmap* get_pic();
    QRect bounded_rect()override;
    bool redo(int index = 0) override;
    bool undo(int index = 0) override;
    QString get_name() override;
    bool test_drag(QPoint point);//测试鼠标按下的点是否是在可伸缩区域中
    void point_move(QPoint point);
    void point_end(QPoint point);
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
    QRect base_rect;
    QRect bound;
    QPoint begin_point;//在图中显示位置的偏移
    QPoint start_point;//用来计算偏移量
    QPoint move_begin_point;
    Paint_layer* paint_layer;
    DragPos drag_pos;
    void paint_pic(QImage& image);
    void corner_point_move(int index, int dx, int dy);
};

#endif // PICTURE_LAYER_H
