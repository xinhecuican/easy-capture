#ifndef ILAYER_H
#define ILAYER_H
#include<QImage>
#include "paint_data.h"
#include "Base/Record_element.h"
class Ilayer : public Record_element
{
public:
    virtual void erase_and_paint(QPoint point, QImage& image)=0;
    virtual void paint(QImage& iamge)=0;
    virtual int add_data(Paint_data* style, QPainterPath path)=0;
    virtual void set_name(QString name)=0;
    virtual QRect bounded_rect()=0;
    virtual bool redo(int index = 0) override{return true;};
    virtual bool undo(int index = 0) override{return true;};
    virtual QString get_name()=0;
};

#endif // ILAYER_H
