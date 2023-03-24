#ifndef PAINTDELETERECORD_H
#define PAINTDELETERECORD_H
#include "Base/RecordElement.h"
#include "Paint/Widgets/Layers/paint_layer.h"
#include "Paint/Widgets/Layers/LayerItems/paintitem.h"

class PaintDeleteRecord : public RecordElement
{
public:
    PaintDeleteRecord(Paint_layer* layer, PaintItem* item, const char* func);
    void redo() override;
    void undo() override;
private:
    Paint_layer* layer;
    PaintItem* item;
    const char* func;
};

#endif // PAINTDELETERECORD_H
