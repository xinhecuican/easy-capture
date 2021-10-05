#ifndef ILAYER_H
#define ILAYER_H
#include<QImage>
#include "paint_data.h"
#include "Base/Record_element.h"
#include<QPixmap>
#include "Common.h"
#include<QDebug>
#include<QPainter>
#include<QWidget>

class Ilayer : public QWidget
{
public:
    Ilayer(QWidget* parent=nullptr):QWidget(parent){};
    virtual void paint(QPainter* painter, QList<QColor> disable_color, bool is_save)=0;
    virtual void set_name(QString name)=0;
    virtual QRect bounded_rect()=0;
    virtual QString get_name()=0;
    virtual ~Ilayer(){};
    virtual void get_focus(){};
    virtual void lose_focus(){};

    virtual void mouse_enter(int key_code){};
    virtual void mouse_move(int dx, int dy){};
    virtual void mouse_release(){};
    virtual void double_click(){};

    virtual void on_size_change(int index, int dx, int dy){};
    virtual void on_paint_change(int index, paint_info info){};
signals:
    void update_rect(QRect rect);
};

#endif // ILAYER_H
