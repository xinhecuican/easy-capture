#ifndef SCROLL_HANDLE_H
#define SCROLL_HANDLE_H
#include<QObject>
#include<QThread>
#include "scroll_worker.h"
#include<QImage>
#include "opencv2/core.hpp"

class Scroll_handle : public QObject
{
    Q_OBJECT
public:
    Scroll_handle(QObject* object=nullptr);
    ~Scroll_handle();
    void start(int begin, int end, QImage base_image, QImage bind_image, int img_height);
    bool is_connect = false;
signals:
    void start_work(QImage scroll_image1, QImage scroll_image2, int img_height);
    void finish(Scroll_handle* handle, int begin, int end, QImage image);
public slots:
    void handle_result(QImage image);
private:
    int begin;
    int end;
    QThread thread;
    Scroll_worker* worker;
};

#endif // SCROLL_HANDLE_H
