#ifndef UPDATEDOWNLOADER_H
#define UPDATEDOWNLOADER_H
#include <QObject>
#include "UpdateData.h"
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QTimer>
#include <QPointer>

class UpdateDownloader : public QObject
{
    Q_OBJECT
public:
    UpdateDownloader(QList<UpdateData> data, QObject* parent=nullptr);
    void start();
signals:
    void success();
    void failure();
private:
    void startInner(QUrl url);
    QList<UpdateData> data;
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
