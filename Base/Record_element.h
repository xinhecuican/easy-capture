#ifndef RECORD_ELEMENT_H
#define RECORD_ELEMENT_H
class Record_element
{
public:
    //undo操作成功返回true
    virtual bool undo(int index=0)=0;
    virtual bool redo(int index=0)=0;
    virtual ~Record_element(){};
};

#endif // RECORD_ELEMENT_H
