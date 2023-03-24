#ifndef SHAPERECORD_H
#define SHAPERECORD_H
#include "Base/RecordElement.h"
#include <QGraphicsObject>

class ShapeRecord : public RecordElement
{
public:
    ShapeRecord(QGraphicsObject* object, QGraphicsObject* show_object, const char* func_name, bool origin_show);
    void redo() override;
    void undo() override;
private:
    QGraphicsObject* object;
    const char* func_name;
    bool show;
};

#endif // SHAPERECORD_H
