#include "shaperecord.h"
#include "Helper/Reflect.h"

ShapeRecord::ShapeRecord(QGraphicsObject* object, QGraphicsObject* show_object, const char* func_name, bool show)
{
    baseObject = object;
    this->object = show_object;
    this->func_name = func_name;
    this->show = show;
}

void ShapeRecord::undo()
{
    Reflect::invokeMethod(baseObject->metaObject()->className(), baseObject, func_name, QGenericReturnArgument(),
                              Q_ARG(bool, show), Q_ARG(QGraphicsObject*, object));
}

void ShapeRecord::redo()
{
    Reflect::invokeMethod(baseObject->metaObject()->className(), baseObject, func_name, QGenericReturnArgument(),
                              Q_ARG(bool, !show), Q_ARG(QGraphicsObject*, object));
}
