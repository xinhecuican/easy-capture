#include "resize_record.h"
#include "Helper/Reflect.h"

ResizeRecord::ResizeRecord(QGraphicsObject* object, const char* func_name, QRectF before_rect, QRectF after_rect)
{
    base_object = object;
    this->object = object;
    this->func_name = func_name;
    this->before_rect = before_rect;
    this->after_rect = after_rect;
    this->base_object = object;
}

void ResizeRecord::redo()
{
    Reflect::invokeMethod(object->metaObject()->className(), object, func_name, QGenericReturnArgument(),
                          Q_ARG(bool, false), Q_ARG(QRectF, before_rect), Q_ARG(QRectF, after_rect));
}

void ResizeRecord::undo()
{
    Reflect::invokeMethod(object->metaObject()->className(), object, func_name, QGenericReturnArgument(),
                          Q_ARG(bool, true), Q_ARG(QRectF, before_rect), Q_ARG(QRectF, after_rect));
}
