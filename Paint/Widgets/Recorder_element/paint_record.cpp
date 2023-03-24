#include "paint_record.h"
#include<QDebug>

PaintRecord::PaintRecord(PaintItem* item)
{
    this->item = item;
    this->baseObject = item;
}

void PaintRecord::undo()
{
    item->hide();
}

void PaintRecord::redo()
{
    item->show();
}
