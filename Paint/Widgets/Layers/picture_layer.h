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
#include "Paint/Data/stretch_button.h"
#include "Helper/plist.h"
#include "Paint/Data/button_group.h"

class Picture_layer : public Ilayer
{
public:
    enum DragPos{top_left, top_right, down_left, down_right};
    Picture_layer();
    Picture_layer(QString name, QPixmap picture, QRect rect, QWidget* parent=nullptr);
    ~Picture_layer();
    void paint(QPainter* painter, QList<QColor> disable_color, bool is_save)override;
    void set_name(QString name) override;
    QPixmap get_pic();
    QRect bounded_rect()override;
    void on_size_change(int index, int dx, int dy) override;
    QString get_name() override;
    void main_layer_paint(QPainter* painter, QList<QColor> disable_color, bool is_save, int scroll_pos);
    bool is_base_layer;
public slots:
    void on_button_move(direction dir, int dx, int dy);
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
    QRect bound;
    QRect pic_rect;
    QPoint move_begin_point;
    DragPos drag_pos;
    Button_group* button_group;
    void paint_pic(QPainter* painter, QList<QColor> disable_color, bool is_save);
};

#endif // PICTURE_LAYER_H
