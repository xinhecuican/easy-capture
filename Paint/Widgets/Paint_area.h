#ifndef PAINT_AREA_H
#define PAINT_AREA_H
#include<QWidget>
#include "Paint/Widgets/Layers/picture_layer.h"
#include<QBoxLayout>
#include<QVector>
#include "Paint/Widgets/Layers/paint_layer.h"
#include "Paint/Data/Ilayer.h"
#include "Paint/Data/Common.h"

class Paint_area : public QWidget
{
public:
    Paint_area();
    ~Paint_area();
    Paint_area(QWidget* parent=nullptr);
    void set_picture(QPixmap pixmap, QRect rect);
    void paintEvent(QPaintEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void mouseDoubleClickEvent(QMouseEvent *event) override;
    void remove_layer(Paint_layer* layer);
    void remove_layer(int index);
    QString create_layer();
    QStringList layers_name();
    void set_name(int index, QString text);
    void set_paintable(bool paintable);
    void raise_layer(int index);//提升一层
    void drop_layer(int index);//下降一层
    void reset();
    void paint_rect(QRect rect);
    void save(QString path);//不可编辑，直接输出图片
    void save_temp();//可供下次编辑，保存了paint_layer
    QImage get_image();
    void using_erase(bool is_using_eraser);
    void set_disable_color(int index, QColor color = QColor());
    bool contain_picture();
    QRect bounded_rect();
    void paint_shape(shape_type type);
    void append_layer(Ilayer* layer);
    bool pic_save;
private:
    QWidget* parent;
    Picture_layer* pic_layer;
    Paint_layer* paint_layer;
    QVBoxLayout* layout;
    QImage image;
    QRect image_bound;
    QVector<Ilayer*> layers;
    Ilayer* focus_layer;
    QList<QColor> disable_color;
    int layer_num;
    bool is_draw;
    bool is_eraser;
    bool is_save;
    bool is_paintable;
    bool is_paint_shape;
    shape_type shape;
    QPainterPath now_path;
    Paint_data now_data;
    QPoint point;

    void paint(QPainter* painter, QList<QColor> disable_color);
    void update_image(QRect bound_rect);
    void find_focus(QPoint point);
};

#endif // PAINT_AREA_H
