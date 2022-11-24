#ifndef MPOOL_H
#define MPOOL_H
#include<QList>

template <class T>
class MPool
{
public:
    MPool();
    T* take();
    void back(T* t);
    void clear();
    int work_count();
private:
    QList<T*> work_list;
    QList<T*> idle_list;
};

template <class T>
MPool<T>::MPool()
{
    work_list = QList<T*>();
    idle_list = QList<T*>();
}

template <class T>
T* MPool<T>::take()
{
    if(idle_list.size() > 0)
    {
        T* last = idle_list.takeLast();
        idle_list.removeLast();
        work_list.push_back(last);
        return last;
    }
    else
    {
        T* t = new T();
        work_list.push_back(t);
        return t;
    }
}

template <class T>
void MPool<T>::back(T* t)
{
    if(work_list.contains(t))
    {
        work_list.removeOne(t);
    }
    idle_list.push_back(t);
}

template <class T>
int MPool<T>::work_count()
{
    return work_list.size();
}

template <class T>
void MPool<T>::clear()
{
    for(T* t : work_list)
    {
        delete t;
    }
    for(T* t : idle_list)
    {
        delete t;
    }
    work_list.clear();
    idle_list.clear();
}

#endif // MPOOL_H
