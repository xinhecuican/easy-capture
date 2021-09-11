#include "resize_record.h"

Resize_record::Resize_record(Ilayer* layer, int index, int dx, int dy)
{
    this->layer = layer;
    this->index = index;
    this->dx = dx;
    this->dy = dy;
}

bool Resize_record::redo(int index)
{
    layer->on_size_change(this->index, dx, dy);
    return true;
}

bool Resize_record::undo(int index)
{
    layer->on_size_change(this->index, -dx, -dy);
    return true;
}
