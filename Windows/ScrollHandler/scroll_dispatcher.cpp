#include "scroll_dispatcher.h"
#include<QColor>
#include<QImage>
#include<QDebug>
#include "Scroll_handler_global.h"

Scroll_dispatcher::Scroll_dispatcher(QObject* object) : QObject(object)
{
    qRegisterMetaType<QImage>("QImage");
    worker = new Dispatcher_worker();
    wakeup_timer = new QTimer();

    worker->moveToThread(&thread);
    index = 0;
    connect(wakeup_timer, &QTimer::timeout, worker, &Dispatcher_worker::check_work);
    connect(this, &Scroll_dispatcher::start_work, worker, &Dispatcher_worker::receive_data);
    connect(worker, &Dispatcher_worker::finish, this, [=](QImage image){
        wakeup_timer->stop();
        emit finish(image);
        thread.quit();
        thread.wait();
    });
    connect(&thread, &QThread::finished, worker, &Dispatcher_worker::on_thread_end);
    connect(this, &Scroll_dispatcher::on_all_images_get, worker, &Dispatcher_worker::get_all_images);
    thread.start();
    wakeup_timer->start(1000);
}

Scroll_dispatcher::~Scroll_dispatcher()
{
    if(Scroll_handler_global::instance() != NULL)
    {
        delete Scroll_handler_global::instance();
    }
    wakeup_timer->stop();
    delete wakeup_timer;
    thread.quit();
    thread.wait();
}

void Scroll_dispatcher::start(QImage image)
{
    if(!thread.isRunning())
    {
        thread.start();
    }
    emit start_work(image, index, index);
    index++;
}

void Scroll_dispatcher::get_all_images()
{
    emit on_all_images_get();
}
