#ifndef PAINT_AREA_H
#define PAINT_AREA_H
#include<QWidget>
#include "picture_layer.h"
#include<QBoxLayout>
#include<QVector>
#include "paint_layer.h"
#include "Paint/Data/Ilayer.h"

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
    QString create_layer();
    QStringList layers_name();
    void set_name(int index, QString text);
    void raise_layer(int index);//提升一层
    void drop_layer(int index);//下降一层
    void reset();
    void save(QString path);//不可编辑，直接输出图片
    void save_temp();//可供下次编辑，保存了paint_layer
    void using_erase(bool is_using_eraser);
    void set_disable_color(int index, QColor color = QColor());
    bool contain_picture();
    QRect bounded_rect();
    bool pic_save;
private:
    QWidget* parent;
    Picture_layer* pic_layer;
    QVBoxLayout* layout;
    QImage image;
    QVector<Ilayer*> layers;
    QList<QColor> disable_color;
    int layer_num;
    bool is_draw;
    bool is_eraser;
    QPainterPath now_path;
    Paint_data now_data;
    QPoint point;
    bool is_base_drag;
    void paint();
};

#endif // PAINT_AREA_H
