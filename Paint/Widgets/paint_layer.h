#ifndef PAINT_LAYER_H
#define PAINT_LAYER_H
#include<QString>
#include<QPicture>
#include<QHash>
#include<QPainterPath>
#include "Base/Record_element.h"
#include "Paint/Data/paint_data.h"
#include "Paint/Data/Ilayer.h"
#include<QWidget>
class Paint_layer : public QWidget, public Ilayer
{
public:
    Paint_layer();
    Paint_layer(QWidget* parent, QString name);
    void paint(QImage &image, QRect rect =QRect(-1, -1, 1, 1)) override;
    void erase_and_paint(QPoint point, QImage& image, QRect rect =QRect(-1, -1, 1, 1))override;
    int add_data(Paint_data* style, QPainterPath path)override;
    void set_name(QString name)override;
    QString get_name();
    QRect bounded_rect()override;
    bool redo(int index = 0) override;
    bool undo(int index = 0) override;
    QPoint begin;
private:
    struct paint_info
    {
        paint_info(){};
        paint_info(Paint_data* style, QPainterPath paint_path)
        {
            style_info = style;
            path = paint_path;
        }
        Paint_data* style_info;
        QPainterPath path;
    };
    QWidget* parent;
    QString name;
    QPicture pic;
    QRect last_rect;
    QHash<int, paint_info> data;
    QHash<int, paint_info> delete_data;
    int now_index;
};

#endif // PAINT_LAYER_H
