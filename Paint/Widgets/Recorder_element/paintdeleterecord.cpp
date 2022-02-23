#include "paintdeleterecord.h"
#include "Helper/Reflect.h"

PaintDeleteRecord::PaintDeleteRecord(Paint_layer* layer, PaintItem* item, const char* func_name)
{
    item->hide();
    this->layer = layer;
    this->item = item;
    this->func = func_name;
    this->base_object = item;
}

void PaintDeleteRecord::undo()
{
    Reflect::invokeMethod(layer->metaObject()->className(), layer, func,
                          Q_ARG(bool, true), Q_ARG(PaintItem*, item));
}

void PaintDeleteRecord::redo()
{
    Reflect::invokeMethod(layer->metaObject()->className(), layer, func,
                          Q_ARG(bool, false), Q_ARG(PaintItem*, item));
}
