#ifndef PAINT_RECORD_H
#define PAINT_RECORD_H
#include "Base/RecordElement.h"
#include "Paint/Data/Common.h"
#include "Paint/Widgets/Layers/LayerItems/paintitem.h"

class PaintRecord : public RecordElement
{
public:
    PaintRecord(PaintItem* item);
    void redo() override;
    void undo() override;
private:
    PaintItem* item;
};

#endif // PAINT_RECORD_H
