#include "recorder.h"
#include<QDebug>

Recorder::Recorder()
{
    data = MVector();
}

Recorder* Recorder::_instance = NULL;

Recorder* Recorder::instance()
{
    if(_instance == NULL)
    {
        _instance = new Recorder();
    }
    return _instance;
}

Recorder::~Recorder()
{
    data.clear();
    _instance = NULL;
}

void Recorder::back()
{
    RecordElement* element;
    //当没到栈底并且当前undo操作不成功时
    if (data.can_undo())
    {
        element = data.pop();
        element->undo();
    }
    emit recordChange();
}

void Recorder::forward()
{
    RecordElement* element;
    if (data.can_redo())
    {
        element = data.forward();
        element->redo();
    }
    emit recordChange();
}

void Recorder::removeRecord(QObject* layer)
{
    for(int i=0; i<data.size(); i++)
    {
        if(data.get(i)->baseObject == layer)
        {
            data.remove(i);
            i--;
        }
    }
    emit recordChange();
}

void Recorder::record(RecordElement* element)
{
    data.push_and_resize(element);
    emit recordChange();
}


void Recorder::reset()
{
    data.reset();
    emit recordChange();
}

bool Recorder::undoAvaliable()
{
    return data.can_undo();
}

bool Recorder::redoAvaliable()
{
    return data.can_redo();
}
