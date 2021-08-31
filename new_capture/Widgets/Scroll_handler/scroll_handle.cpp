#include "scroll_handle.h"
#include<QImage>
#include<QDebug>
#include<QDateTime>

Scroll_handle::Scroll_handle(QObject* object) : QObject(object)
{
    worker = new Scroll_worker();
    worker->moveToThread(&thread);
    connect(this, &Scroll_handle::start_work, worker, &Scroll_worker::begin_work);
    connect(worker, &Scroll_worker::work_finish, this, &Scroll_handle::handle_result);
    connect(&thread, &QThread::finished, worker, &QObject::deleteLater);
    thread.start();
}

Scroll_handle::~Scroll_handle()
{
    thread.quit();
    thread.wait();
}

void Scroll_handle::handle_result(QImage image)
{

    emit finish(this, begin, end, image);
}

void Scroll_handle::start(int begin, int end, QImage base_image, QImage bind_image, int img_height)
{
    this->begin = begin;
    this->end = end;
    emit start_work(base_image, bind_image, img_height);
}
