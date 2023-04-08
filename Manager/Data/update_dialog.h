#ifndef UPDATE_DIALOG_H
#define UPDATE_DIALOG_H
#include<QDialog>
#include "UpdateData.h"
#include <QtNetwork>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include "Reply_timeout.h"
#include "updatedownloader.h"
class Update_dialog : public QDialog
{
    Q_OBJECT
public:
    Update_dialog();
    Update_dialog(QList<UpdateData> data, QWidget* widget);
    ~Update_dialog();
signals:
    void download_finished();
private:
    UpdateDownloader* downloader;
};

#endif // UPDATE_DIALOG_H
