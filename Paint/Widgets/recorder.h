#ifndef Recorder_H
#define Recorder_H
#include<QVector>
#include "Base/Record_element.h"
#include<iostream>
#include "Helper/debug.h"
#include<QGraphicsItem>
#include<QObject>

class Recorder : public QObject
{
    Q_OBJECT
public:
    Recorder();
    ~Recorder();
    static Recorder* instance();
    void record(RecordElement* element);
    void reset();
    void back();
    void forward();
    void removeRecord(QObject* item);
    bool undoAvaliable();
    bool redoAvaliable();
signals:
    void recordChange();
private:
    struct Record_data
    {
        Record_data(){};
        Record_data(int index, RecordElement* element)
        {
            this->index = index;
            this->element = element;
        }
        int index;//在element中的标识
        RecordElement* element;
    };
    struct MVector
    {
        int capacity;
        int limit;
        int point;
        int max_size;
        RecordElement** data;
        MVector()
        {
            capacity = 10;
            limit = 0;
            point = 0;
            max_size = 0;
            data = new RecordElement*[10];
        }

        void ensure_capacity()
        {
            if(point+1 >= capacity)
            {
                int old_size = capacity;
                capacity <<= 1;
                RecordElement** temp = new RecordElement*[capacity];
                for(int i=0; i<old_size; i++)
                {
                    temp[i] = data[i];
                }
                delete [] data;
                data = temp;
            }
        }

        void clear()
        {
            for(int i=0; i<max_size; i++)
            {
                delete data[i];
            }
            delete [] data;
        }

        void reset()
        {
            for(int i=0; i<max_size; i++)
            {
                delete data[i];
            }
            RecordElement** temp = new RecordElement*[10];
            capacity = 10;
            limit = 0;
            point = 0;
            max_size = 0;
            delete [] data;
            data = temp;
        }

        void push_back(RecordElement* record_data)
        {
            ensure_capacity();
            data[point++] = record_data;
            if(point > max_size)
            {
                max_size = point;
            }
            limit++;
        }

        RecordElement* pop()
        {
            if(point > 0)
            {
                return data[--point];
            }
            Debug::debug_print_warning("下标过小\n位置Recorder::MVector::pop()");
            return data[0];
        }

        void push_and_resize(RecordElement* record_data)
        {
            data[point++] = record_data;
            limit = point;
        }

        int remove(int i)
        {
            if(i < limit)
            {
                if(i <= point)
                {
                    point--;
                }
                for(int k=i; k<limit-1; k++)
                {
                    data[k] = data[k+1];
                }
                limit--;
            }
            return i;
        }

        RecordElement* get(int i)
        {
            if(i > limit-1)
            {
                Debug::debug_print_warning("数组下标超界\n位置Recorder::MVector::get()");
                return data[0];
            }
            return data[i];
        }

        inline bool can_redo(){return point < limit;};
        inline bool can_undo(){return point > 0;};

        inline int size(){return limit;};

        inline RecordElement*& forward(){point++;return data[point-1];};
    };

    static Recorder* _instance;
    MVector data;
};

#endif // Recorder_H
