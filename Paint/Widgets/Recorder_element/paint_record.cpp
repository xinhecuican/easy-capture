#include "paint_record.h"
#include<QDebug>

Paint_record::Paint_record(Ilayer* layer, int index, paint_info info)
{
    this->layer = layer;
    this->index = index;
    this->info = info;
}

bool Paint_record::redo(int index)
{
    layer->on_paint_change(this->index, this->info);
    return true;
}

bool Paint_record::undo(int index)
{
    layer->on_paint_change(this->index, this->info);
    return true;
}
