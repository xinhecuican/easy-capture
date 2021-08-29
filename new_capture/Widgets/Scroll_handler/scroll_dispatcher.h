#ifndef SCROLL_DISPATCHER_H
#define SCROLL_DISPATCHER_H
#include<QObject>
#include<QThread>
#include "dispatcher_worker.h"
#include<QList>

class Scroll_dispatcher : public QObject
{
    Q_OBJECT
public:
    Scroll_dispatcher(QObject* object=nullptr);
    ~Scroll_dispatcher();
    void start(QImage image);
    void get_all_images();
signals:
    void start_work(QImage image, int begin, int end);
    void finish(QImage image);
    void on_all_images_get();
    void stop_timer();
private:
    QTimer* wakeup_timer;
    bool contains_begin;
    bool contains_end;
    QThread thread;
    Dispatcher_worker* worker;
    int index;
};

#endif // SCROLL_DISPATCHER_H
