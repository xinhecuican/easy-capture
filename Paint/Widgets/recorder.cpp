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
    Record_base* element;
    //当没到栈底并且当前undo操作不成功时
    while (data.can_undo())
    {
        element = data.pop();
        if(element->undo(0))
        {
            break;
        }
    }


}

void Recorder::forward()
{
    Record_base* element;
    while (data.can_redo())
    {
        element = data.forward();
        if(element->redo(0))
        {
            break;
        }
    }
}

void Recorder::remove_record(Ilayer *layer)
{
    for(int i=0; i<data.size(); i++)
    {
        if(data.get(i)->layer == layer)
        {
            data.remove(i);
            i--;
        }
    }
}

void Recorder::record(Record_base* element)
{
    data.push_and_resize(element);
}


void Recorder::reset()
{
    data.reset();
}
