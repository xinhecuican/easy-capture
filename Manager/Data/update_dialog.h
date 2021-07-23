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
public:
    Update_dialog();
    Update_dialog(Update_data data, QWidget* widget);
private:
    Update_data data;
    QNetworkRequest request;
    QNetworkAccessManager manager;
    QNetworkReply* reply;
    Reply_timeout* timeout;
};

#endif // UPDATE_DIALOG_H
