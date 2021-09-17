#ifndef UPDATE_DIALOG_H
#define UPDATE_DIALOG_H
#include<QDialog>
#include "update_data.h"
#include <QtNetwork>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include "Reply_timeout.h"
class Update_dialog : public QDialog
{
    Q_OBJECT
public:
    Update_dialog();
    Update_dialog(QList<Update_data> data, QWidget* widget);
signals:
    void download_finished();
private:
    QList<Update_data> data;
    QList<QString> file_names;
    QNetworkRequest request;
    QNetworkAccessManager manager;
    QNetworkReply* reply;
    Reply_timeout* timeout;
    int update_sum;
    void get_one_update();
};

#endif // UPDATE_DIALOG_H
