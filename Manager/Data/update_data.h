#ifndef UPDATE_DATA_H
#define UPDATE_DATA_H
#include "Base/Serializable.h"
#include<QDateTime>
#include<QDebug>

class Update_data : Serializable
{
public:
    Update_data();
    Update_data(QString version, QString url, QString time, QString description);
    void serialized(QJsonObject *json) override;
    void deserialized(QJsonObject *json) override;
    void decompress_version(int* sum);
    void set_version(QString version);
    void set_description(QString description);
    void set_url(QString url);
    void set_time(QString time);
    QString get_version();
    QString get_url();
    QString get_time();
    QString get_description();
    QDateTime to_datetime();
    bool operator<(Update_data& data)
    {
         int sum1, sum2;
         decompress_version(&sum1);
         data.decompress_version(&sum2);
         return sum1 < sum2;
    }

    bool operator>(Update_data& data)
    {
        int sum1, sum2;
        decompress_version(&sum1);
        data.decompress_version(&sum2);
        return sum1 > sum2;
    }
    bool operator ==(Update_data& data)
    {
        int sum1, sum2;
        decompress_version(&sum1);
        data.decompress_version(&sum2);
        return sum1 == sum2;
    }
private:
    QString version;
    QString description;
    QString url;
    QString time;
};

#endif // UPDATE_DATA_H
