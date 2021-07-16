#include "update_data.h"

Update_data::Update_data()
{

}

Update_data::Update_data(QString version, QString url, QString time, QString description)
{
    this->version = version;
    this->time = time;
    this->url = url;
    this->description = description;
}

void Update_data::set_url(QString url)
{
    this->url = url;
}

void Update_data::set_time(QString time)
{
    this->time = time;
}

void Update_data::set_version(QString version)
{
    this->version = version;
}

void Update_data::set_description(QString description)
{
    this->description = description;
}

QString Update_data::get_version()
{
    return version;
}

QString Update_data::get_time(){return time;}
QString Update_data::get_url(){return url;}
QString Update_data::get_description(){return description;}

void Update_data::deserialized(QJsonObject *json)
{
    time = (*json)["time"].toString();
    description = (*json)["description"].toString();
    url = (*json)["url"].toString();
    version = (*json)["version"].toString();
}

void Update_data::serialized(QJsonObject *json)
{
    json->insert("time", time);
    json->insert("description", description);
    json->insert("url", url);
    json->insert("version", version);
}

void Update_data::decompress_version(int *sum)
{
    int begin = 0;
    int ans = 0;
    while(true)
    {
        int temp = version.indexOf('.', begin);
        if(temp == -1)
        {
            break;
        }
        ans *= 10;
        ans += version.mid(begin, temp-begin).toInt();
        begin = temp + 1;
    }
    *sum = ans;
}

QDateTime Update_data::to_datetime()
{
    return QDateTime::fromString(time);
}
