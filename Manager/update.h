#ifndef UPDATE_H
#define UPDATE_H
#include "Base/Serializable.h"
#include "Data/update_data.h"
#include<QNetworkReply>
#include<QWidget>

class Update : QWidget, Serializable
{
public:
    Update();
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
    void serialized(QJsonObject *json) override;
    void deserialized(QJsonObject *json) override;
    static Update_data now_version;
private:
    static Update* _instance;
    Update_data newest_data;
    QNetworkReply* reply;
    QNetworkRequest request;
    void start_request(const QUrl& url);
};

#endif // UPDATE_H
