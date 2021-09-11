#ifndef ILAYER_H
#define ILAYER_H
#include<QImage>
#include "paint_data.h"
#include "Base/Record_element.h"
#include<QPixmap>
#include "Common.h"
#include<QDebug>
#include<QPainter>

class Ilayer
{
public:
    virtual void erase_and_paint(QPoint point, QPainter* painter, QList<QColor> disable_color)=0;
    virtual void paint(QPainter* painter, QList<QColor> disable_color, bool is_save)=0;
    virtual int add_data(Paint_data* style, QPainterPath path)=0;
    virtual void set_name(QString name)=0;
    virtual QRect bounded_rect()=0;
    virtual QString get_name()=0;
    virtual ~Ilayer(){};

    virtual void on_size_change(int index, int dx, int dy){};
    virtual void on_paint_change(int index, paint_info info){};
signals:
    void update_rect(QRect rect);
};

#endif // ILAYER_H
