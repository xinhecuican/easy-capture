#ifndef RECORD_ELEMENT_H
#define RECORD_ELEMENT_H
class Record_element
{
public:
    virtual void undo(int index=0)=0;
    virtual void redo(int index=0)=0;
    virtual ~Record_element(){};
};

#endif // RECORD_ELEMENT_H
