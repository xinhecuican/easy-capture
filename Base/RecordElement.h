#ifndef RECORD_ELEMENT_H
#define RECORD_ELEMENT_H
#include<QObject>
class RecordElement {
public:
    /// undo操作成功返回true
    virtual void undo()=0;
    virtual void redo()=0;
    virtual ~RecordElement() {}
    QObject* baseObject;
};

#endif // RECORD_ELEMENT_H
