#ifndef RESIZE_RECORD_H
#define RESIZE_RECORD_H
#include "record_base.h"
#include "Paint/Data/Ilayer.h"

class Resize_record : public Record_base
{
public:
    Resize_record(Ilayer* layer, int index, int dx, int dy);
    bool redo(int index = 0) override;
    bool undo(int index = 0) override;
private:
    int index;
    int dx;
    int dy;
};

#endif // RESIZE_RECORD_H
