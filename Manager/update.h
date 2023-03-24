#ifndef UPDATE_H
#define UPDATE_H
#include "Base/Serializable.h"
#include "Data/update_data.h"
#include "Data/update_dialog.h"
#include<QNetworkReply>
#include<QWidget>

class Update : public QWidget, Serializable {
    Q_OBJECT
public:
    enum update_state_t {IDLE, CHECKING, UPDATING};
    Update();
    ~Update();
    static Update* instance() {
        if(_instance == NULL) {
            _instance = new Update();
        }
        return _instance;
    }
    void checkUpdate();
    void save();
    void load();
    void update();
    void serialized(QJsonObject *json) override;
    void deserialized(QJsonObject *json) override;
    void onFinish();
    static Update_data now_version;
signals:
    void updateStateChange(update_state_t);
private slots:
    void on_update();
private:
    update_state_t updateState;
    static Update* _instance;
    Update_data newest_data;
    QPointer<QNetworkReply> reply;
    QNetworkRequest request;
    void start_request(const QUrl& url);
    QNetworkAccessManager* manager;
    Update_dialog* dialog;
    QList<Update_data> data_list;
    int reconnect_times;
    QTimer* timer;
    QTimer* timeoutTimer;
    qint64 currentReceive;
    qint64 timerReceive;
};

#endif // UPDATE_H
