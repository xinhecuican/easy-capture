#ifndef UPDATEDOWNLOADER_H
#define UPDATEDOWNLOADER_H
#include <QObject>
#include "update_data.h"
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QTimer>
#include <QPointer>

class UpdateDownloader : public QObject
{
    Q_OBJECT
public:
    UpdateDownloader(QList<Update_data> data, QObject* parent=nullptr);
    void start();
signals:
    void success();
    void failure();
private:
    QList<Update_data> data;
    QNetworkRequest request;
    QNetworkAccessManager manager;
    QPointer<QNetworkReply> reply;
    int updateSum;
    qint64 currentReceive;
    qint64 timerReceive;
    QTimer* timeoutTimer;
    QList<QString> fileNames;
};

#endif // UPDATEDOWNLOADER_H
