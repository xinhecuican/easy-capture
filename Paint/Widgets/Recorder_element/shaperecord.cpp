#include "shaperecord.h"
#include "Helper/Reflect.h"

ShapeRecord::ShapeRecord(QGraphicsObject* object, QGraphicsObject* show_object, const char* func_name, bool show)
{
    base_object = object;
    this->object = show_object;
    this->func_name = func_name;
    this->show = show;
}

void ShapeRecord::undo()
{
    Reflect::invokeMethod(base_object->metaObject()->className(), base_object, func_name, QGenericReturnArgument(),
                              Q_ARG(bool, show), Q_ARG(QGraphicsObject*, object));
}

void ShapeRecord::redo()
{
    Reflect::invokeMethod(base_object->metaObject()->className(), base_object, func_name, QGenericReturnArgument(),
                              Q_ARG(bool, !show), Q_ARG(QGraphicsObject*, object));
}
