#ifndef PAINT_AREA_H
#define PAINT_AREA_H
#include<QWidget>
#include "picture_layer.h"
#include<QBoxLayout>
#include<QVector>
#include "paint_layer.h"

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
    void remove_layer(Paint_layer* layer);
    void remove_layer(int index);
    void create_layer();
    void raise_layer(Paint_layer* layer);//提升一层
    void drop_layer(Paint_layer* layer);//下降一层
    void reset();
private:
    QWidget* parent;
    Picture_layer* pic_layer;
    QVBoxLayout* layout;
    QImage image;
    QVector<Paint_layer*> layers;
    int layer_num;
    bool is_draw;
    QPainterPath now_path;
    Paint_data now_data;
};

#endif // PAINT_AREA_H
