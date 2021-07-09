#include "recorder.h"

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
    Record_data element;
    //当没到栈底并且当前undo操作不成功时
    while(data.can_undo() && !(element = data.pop()).element->undo(element.index)){}
}

void Recorder::forward()
{
    Record_data element;
    while(data.can_redo() && !(element = data.forward()).element->redo(element.index)){};
    /*if(data.can_redo())
    {
        Record_data element = data.forward();
        element.element->redo(element.index);
    }*/
}

void Recorder::record(int index, Record_element *element)
{
    Record_data record_data(index, element);
    data.push_and_resize(record_data);
}

void Recorder::remove_record(Record_element *element)
{
    int i=0;
    while(i < data.size())
    {
        if(data.get(i).element == element)
        {
            i = data.remove(i);
            continue;
        }
        i++;
    }
}

void Recorder::reset()
{
    data.reset();
}
