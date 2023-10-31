#include "debug.h"
#include<QMessageBox>
#include<QDebug>
#include<QMainWindow>
#include<QLabel>
#include<QScrollArea>
#include<QDateTime>
#include <QThread>

QMap<int, QList<Debug::TimerElement>> Debug::timerName = QMap<int, QList<TimerElement>>();
QMutex* Debug::mutex = new QMutex;

void Debug::show_error_message(QString message) {
    qCritical() << message;
}

void Debug::debug_print_warning(QString message) {
    qWarning() << message;
}

void Debug::showImage(QImage image) {
    QMainWindow mw;

    QLabel *label=new QLabel();
    label->setPixmap(QPixmap::fromImage(image));

    QScrollArea *sa=new QScrollArea(&mw);
    sa->setWidget(label);
    sa->resize(400,400);
    mw.show();
}

void Debug::beginTimer(QString name) {
    TimerElement timerElement;
    timerElement.name = name;
    timerElement.beginTime = QDateTime::currentMSecsSinceEpoch();
    int threadId = quintptr(QThread::currentThreadId());
    mutex->lock();
    if(!timerName.contains(threadId)) {
        QList<TimerElement> timerList;
        timerName[threadId] = timerList;
    }
    timerName[threadId].append(timerElement);
    mutex->unlock();
}

void Debug::endTimer() {
    int threadId = quintptr(QThread::currentThreadId());
    mutex->lock();
    if(timerName[threadId].size() == 0)
        return;
    TimerElement timerElement = timerName[threadId].last();
    timerName[threadId].pop_back();
    mutex->unlock();
    qDebug() << "timer " << timerElement.name << ": " << QDateTime::currentMSecsSinceEpoch() - timerElement.beginTime;
}

qint64 Debug::getTime() {
    return QDateTime::currentMSecsSinceEpoch();
}
