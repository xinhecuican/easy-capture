#ifndef PAINTDELETERECORD_H
#define PAINTDELETERECORD_H
#include "Base/RecordElement.h"
#include "Paint/Widgets/Layers/paintlayer.h"
#include "Paint/Widgets/Layers/LayerItems/paintitem.h"

class PaintDeleteRecord : public RecordElement
{
public:
    PaintDeleteRecord( PaintLayer* layer, PaintItem* item, const char* func);
    void redo() override;
    void undo() override;
private:
     PaintLayer* layer;
    PaintItem* item;
    const char* func;
};

#endif // PAINTDELETERECORD_H
