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
#include "Paint/Widgets/Recorder_element/paint_record.h"
#include "Paint/Data/Common.h"

class Paint_layer : public Ilayer
{
public:
    Paint_layer();
    Paint_layer(QWidget* parent, QString name);
    void paint(QPainter* painter, QList<QColor> disable_color, bool is_save) override;
    void erase_and_paint(QPoint point, QPainter* painter, QList<QColor> disable_color);
    int add_data(Paint_data* style, QPainterPath path);
    void set_name(QString name)override;
    QString get_name()override;
    QPolygon bounded_rect()override;
    void on_paint_change(int index, paint_info info) override;
    void clear();
    QPoint begin;
private:

    QWidget* parent;
    QString name;
    QHash<int, paint_info> data;
    int now_index;
};

#endif // PAINT_LAYER_H
