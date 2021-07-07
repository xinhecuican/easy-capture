#include "recoder.h"

Recoder::Recoder()
{
    data = MVector();
}

Recoder* Recoder::_instance = NULL;

Recoder* Recoder::instance()
{
    if(_instance == NULL)
    {
        _instance = new Recoder();
    }
    return _instance;
}

void Recoder::back()
{
    if(data.can_undo())
    {
        Record_data element = data.pop();
        element.element->undo(element.index);
    }
}

void Recoder::forward()
{
    if(data.can_redo())
    {
        Record_data element = data.forward();
        element.element->redo(element.index);
    }
}

void Recoder::record(int index, Record_element *element)
{
    Record_data record_data(index, element);
    data.push_and_resize(record_data);
}

void Recoder::remove_record(Record_element *element)
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
