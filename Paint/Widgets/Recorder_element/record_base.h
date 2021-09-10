#ifndef RECORD_BASE_H
#define RECORD_BASE_H
#include "Base/Record_element.h"

class Record_base : Record_element
{
public:
    int index;
    bool redo(int index = 0) override;
    bool undo(int index = 0) override;
};

#endif // RECORD_BASE_H
