#ifndef DISPATCHER_WORKER_H
#define DISPATCHER_WORKER_H
#include<QObject>
#include<QTimer>
#include "Helper/Pool/mpool.h"
#include "scroll_handle.h"
#include<QMap>
#include<QMutex>
#include "opencv2/core/core.hpp"
#include "Helper/image_helper.h"
using namespace cv;

class Dispatcher_worker : public QObject
{
    Q_OBJECT
public:
    Dispatcher_worker(QObject* object=nullptr);
    void work(QImage image, int begin, int end=-1);
    ~Dispatcher_worker();

public slots:
    void receive_data(QImage image, int begin, int end);
    void get_all_images();
    void on_thread_end();
    void handle_result(Scroll_handle* handle, int begin, int end,
                       QImage ans_image);
    void check_work();
signals:
    void finish(QImage image);
private:

    QMutex mutex;
    QMutex list_mutex;
    struct combine_data
    {
        int begin;
        int end;
        QImage image;
    };

    QList<Scroll_handle*> work_list;
    QList<Scroll_handle*> idle_list;
    int img_width;
    int img_height;
    bool _get_all_images;
    Scroll_handle* take();
    void back(Scroll_handle* handle);
    QMap<int, combine_data> map;
    int thread_count;
    bool is_init;

    void combine(combine_data image1, combine_data image2);
};

#endif // DISPATCHER_WORKER_H
