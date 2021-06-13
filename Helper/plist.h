#ifndef PLIST_H
#define PLIST_H
#include<QList>
#include "qlist.h"

/*
 * 存储指针的列表，删除时会释放内存
 */
template <class T>
class PList : public QList<T>
{
public:
    PList() : QList<T>()
    {

    }
    void remove_one(int i);
    void remove_range(int begin, int end);
    void clear_all ();
};
template <class T>
void PList<T>::remove_one(int i)
{
    delete this->value(i);
    this->removeAt(i);
}

template <class T>
void PList<T>::remove_range(int begin, int end)
{
    if(begin < 0 || end > this->size())
    {
        return;
    }
    auto iter = this->begin();
    for(int i=0; iter != this->end(); i++)
    {
        if(i >= begin && i <= end)
        {
            delete &iter;
            iter = this->erase(iter);
        }
        else if(i > end)
        {
            break;
        }
        else
        {
            iter++;
        }
    }
}

template<class T>
void PList<T>::clear_all()
{
    for(int i=0; i<this->size(); i++)
    {
        delete this->value(i);
    }
    this->clear();
}

#endif // PLIST_H
