#ifndef LAYERRECORD_H
#define LAYERRECORD_H
#include "recordbase.h"
#include "../Layer/layerbase.h"

class LayerRecord : public RecordBase
{
public:
    LayerRecord(bool show, LayerBase* layer, int type);
    void redo() override;
    void undo() override;
    void release() override;
private:
    LayerBase* layer;
    bool show;
};

#endif // LAYERRECORD_H
