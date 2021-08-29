#include "dispatcher_worker.h"
#include<QImage>
#include<QPainter>
#include<QDebug>
#include<windows.h>
#include<QDateTime>
#include "Helper/debug.h"

Dispatcher_worker::Dispatcher_worker(QObject* object) : QObject(object)
{
    work_list = QList<Scroll_handle*>();
    idle_list = QList<Scroll_handle*>();
    _get_all_images = false;
    map = QMap<int, combine_data>();
    is_init = false;
//    connect(&wakeup_timer, &QTimer::timeout, this, [=](){
//        check_work();
//    });
//    wakeup_timer.start(1000);
}

Dispatcher_worker::~Dispatcher_worker()
{
    for(int i=0; i<work_list.size(); i++)
    {
        delete work_list[i];
    }

    for(int i=0; i<idle_list.size(); i++)
    {
        delete idle_list[i];
    }
}

void Dispatcher_worker::on_thread_end()
{
//    if(wakeup_timer.isActive())
//    {
//        wakeup_timer.stop();
//    }
    this->deleteLater();
}

void Dispatcher_worker::work(QImage image, int begin, int end)
{

    QMutexLocker locker(&mutex);
    combine_data data;
    data.begin = begin;
    data.end = end == -1 ? begin : end;
    data.image = image;
    map[begin] = data;
    locker.unlock();
    check_work();
}

void Dispatcher_worker::check_work()
{
    QMutexLocker locker(&mutex);
    if(map.size() >= 2)
    {
        while(map.size() >= 2)
        {
            combine_data image1, image2;
            image1 = map.first();
            bool success = false;
            auto tmp_iter = map.begin();
            for(auto iter=++map.begin(); iter!=map.end(); iter++)
            {
                if(iter.value().begin - image1.end == 1)
                {
                    success = true;
                    image2 = iter.value();
                    map.erase(tmp_iter);
                    map.erase(iter);
                    break;
                }
                image1 = iter.value();
                tmp_iter = iter;
            }
            if(success)
            {
                combine(image1, image2);
            }
            else
            {
                break;
            }
        }
    }
    else
    {
        if(work_list.size() == 0 && map.size() == 1 && _get_all_images)
        {
            emit finish(map.take(map.firstKey()).image);
        }
    }

}

void Dispatcher_worker::combine(combine_data image1, combine_data image2)
{

    Scroll_handle* handle;
    if((handle = take()) != NULL)
    {
        handle->start(image1.begin, image2.end, image1.image, image2.image, img_height);
    }
}

Scroll_handle* Dispatcher_worker::take()
{
    if(idle_list.size() > 0)
    {
        Scroll_handle* last = idle_list.takeLast();
        work_list.push_back(last);
        return last;
    }
    else
    {
        SYSTEM_INFO sysInfo;
        GetSystemInfo(&sysInfo);
        if(idle_list.size() + work_list.size() > (int)sysInfo.dwNumberOfProcessors)//限制线程数量
        {
            return NULL;
        }
        Scroll_handle* t = new Scroll_handle();
        work_list.push_back(t);
        connect(t, &Scroll_handle::finish, this, &Dispatcher_worker::handle_result);
        return t;
    }
}

void Dispatcher_worker::handle_result(Scroll_handle* t, int begin, int end, QImage ans_image)
{
    back(t);
    work(ans_image, begin, end);
}

void Dispatcher_worker::receive_data(QImage image, int begin, int end)
{
    combine_data data;
    data.image = image;
    data.begin = begin;
    data.end = end;
    QMutexLocker locker(&mutex);
    map[begin] = data;
    if(!is_init)
    {
        is_init = true;
        img_width = image.width();
        img_height = image.height();
    }
}


void Dispatcher_worker::back(Scroll_handle *t)
{
    if(work_list.contains(t))
    {
        work_list.removeOne(t);
    }
    idle_list.push_back(t);
}

void Dispatcher_worker::get_all_images()
{
    if(work_list.size() == 0 && map.size() == 1 && _get_all_images)
    {
        emit finish(map.first().image);
        return;
    }
    _get_all_images = true;
}
