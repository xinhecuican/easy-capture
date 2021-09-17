#ifndef UPDATE_H
#define UPDATE_H
#include "Base/Serializable.h"
#include "Data/update_data.h"
#include "Data/update_dialog.h"
#include<QNetworkReply>
#include<QWidget>

class Update : QWidget, Serializable
{
    Q_OBJECT
public:
    Update();
    ~Update();
    static Update* instance()
    {
        if(_instance == NULL)
        {
            _instance = new Update();
        }
        return _instance;
    }
    void check_update();
    void save();
    void load();
    void update();
    void serialized(QJsonObject *json) override;
    void deserialized(QJsonObject *json) override;
    static Update_data now_version;
private slots:
    void on_update();
private:
    static Update* _instance;
    Update_data newest_data;
    QNetworkReply* reply;
    QNetworkRequest request;
    void start_request(const QUrl& url);
    QNetworkAccessManager* manager;
    Update_dialog* dialog;
    QList<Update_data> data_list;
    int reconnect_times;
    QTimer* timer;
};

#endif // UPDATE_H
