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
        int limit;
        int point;
        QVector<RecordElement*> data;
        MVector()
        {
            limit = 0;
            point = 0;
        }

        void clear()
        {
            for(int i=0; i<limit; i++)
            {
                delete data[i];
            }
        }

        void reset()
        {
            for(int i=0; i<limit; i++)
            {
                delete data[i];
            }
            limit = 0;
            point = 0;
        }

        RecordElement* pop()
        {
            if(point > 0)
            {
                return data[--point];
            }
            Debug::debug_print_warning("下标过小\n位置Recorder::MVector::pop()");
            return NULL;
        }

        void push_and_resize(RecordElement* record_data)
        {
            if(can_redo())
            {
                for(int i=point; i<limit; i++)
                {
                    delete data[i];
                }
                data[point++] = record_data;
            }
            else
            {
                data.push_back(record_data);
                point++;
            }
            limit = point;
        }

        int remove(int i)
        {
            if(i < limit)
            {
                delete data[i];
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
                return NULL;
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
