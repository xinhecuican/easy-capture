#ifndef PAINT_RECORD_H
#define PAINT_RECORD_H
#include "record_base.h"
#include "Paint/Data/Common.h"

class Paint_record : public Record_base
{
public:

    Paint_record(Ilayer* layer, int index, paint_info info);
    bool redo(int index = 0) override;
    bool undo(int index = 0) override;
private:
    int index;
    paint_info info;

};

#endif // PAINT_RECORD_H
