#ifndef GRABCUTHANDLER_H
#define GRABCUTHANDLER_H
#include <QObject>
#include <QThread>
#include <QPainterPath>
#include <QImage>

class GrabcutWorker : public QObject {
    Q_OBJECT
public:
    GrabcutWorker(QObject* parent=nullptr);
public slots:
    void work(QImage image, QList<QPainterPath> fgd, QList<QPainterPath> bgd);
signals:
    void finish(QImage mask);
};

class GrabcutHandler : public QObject
{
    Q_OBJECT
public:
    GrabcutHandler(QObject* parent=nullptr);
    ~GrabcutHandler();
    void start(QImage image, QList<QPainterPath> fgd, QList<QPainterPath> bgd);
    bool isFinish();
signals:
    void startWork(QImage image, QList<QPainterPath> fgd, QList<QPainterPath> bgd);
    void finish(QImage mask);

private:
    QThread thread;
    GrabcutWorker* worker;
    bool _finish;
    bool working;
    bool waiting;
    QImage waitImage;
    QList<QPainterPath> waitfgd;
    QList<QPainterPath> waitbgd;
};

#endif // GRABCUTHANDLER_H
