#ifndef RECORDBASE_H
#define RECORDBASE_H


class RecordBase
{
public:
    RecordBase(){type = 0;}
    RecordBase(int index){this->type = index;}
    virtual ~RecordBase(){}
    virtual void undo()=0;
    virtual void redo()=0;
    virtual void release()=0;
    int type;
};

#endif // RECORDBASE_H
