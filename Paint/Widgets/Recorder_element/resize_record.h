#ifndef RESIZE_RECORD_H
#define RESIZE_RECORD_H
#include "Base/Record_element.h"
#include<QGraphicsObject>

class ResizeRecord : public RecordElement
{
public:
    ResizeRecord(QGraphicsObject* object, const char* func_name, QRectF before_rect, QRectF after_rect);
    void redo() override;
    void undo() override;
private:
    QGraphicsObject* object;
    const char* func_name;
    QRectF before_rect;
    QRectF after_rect;
};

#endif // RESIZE_RECORD_H
