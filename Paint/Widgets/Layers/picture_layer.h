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

class Picture_layer : public QWidget, public Ilayer
{
public:
    enum DragPos{top_left, top_right, down_left, down_right};
    Picture_layer();
    Picture_layer(QString name, QPixmap picture, QRect rect, QWidget* parent=nullptr);
    ~Picture_layer();
    void paint(QPainter* painter, QList<QColor> disable_color, bool is_save)override;
    void erase_and_paint(QPoint point, QPainter* painter, QList<QColor> disable_color) override;
    void set_name(QString name) override;
    int add_data(Paint_data *style, QPainterPath path) override;
    QPixmap get_pic();
    QRect bounded_rect()override;
    bool redo(int index = 0) override;
    bool undo(int index = 0) override;
    QString get_name() override;
public slots:
    void on_button_move(Stretch_button::direction dir, int dx, int dy);
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
    Paint_layer* paint_layer;
    DragPos drag_pos;
    PList<Stretch_button*> buttons;
    void paint_pic(QPainter* painter, QList<QColor> disable_color, bool is_save);
};

#endif // PICTURE_LAYER_H
