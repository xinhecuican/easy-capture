#ifndef RECORD_BASE_H
#define RECORD_BASE_H
#include "Base/Record_element.h"
#include "Paint/Data/Ilayer.h"

class Record_base : Record_element
{
public:
    Ilayer* layer;
    virtual bool redo(int index = 0) override=0;
    virtual bool undo(int index = 0) override=0;
    virtual ~Record_base(){};
};

#endif // RECORD_BASE_H
