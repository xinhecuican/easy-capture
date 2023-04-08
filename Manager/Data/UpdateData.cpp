#include "UpdateData.h"

UpdateData::UpdateData() {

}

UpdateData::UpdateData(QString version, QString url, QString time, QString description, bool isBeta) {
    this->version = version;
    this->time = time;
    this->url = url;
    this->description = description;
    this->_isBeta = isBeta;
}

void UpdateData::set_url(QString url) {
    this->url = url;
}

void UpdateData::set_time(QString time) {
    this->time = time;
}

void UpdateData::set_version(QString version) {
    this->version = version;
}

void UpdateData::set_description(QString description) {
    this->description = description;
}

QString UpdateData::get_version() {
    return version;
}

QString UpdateData::get_time() {
    return time;
}
QString UpdateData::get_url() {
    return url;
}
QString UpdateData::get_description() {
    return description;
}

void UpdateData::deserialized(QJsonObject *json) {
    time = (*json)["time"].toString();
    description = (*json)["description"].toString();
    url = (*json)["url"].toString();
    version = (*json)["version"].toString();
    if((*json).find("isBeta") != (*json).end()) {
        _isBeta = (*json)["isBeta"].toBool();
    } else {
        _isBeta = false;
    }
}

void UpdateData::serialized(QJsonObject *json) {
    json->insert("time", time);
    json->insert("description", description);
    json->insert("url", url);
    json->insert("version", version);
    json->insert("isBeta", _isBeta);
}

void UpdateData::decompress_version(int *sum) {
    int begin = 0;
    int ans = 0;
    while(true) {
        int temp = version.indexOf('.', begin);
        if(temp == -1) {
            break;
        }
        ans *= 100;
        ans += version.mid(begin, temp-begin).toInt();
        begin = temp + 1;
    }
    ans *= 100;
    ans += version.mid(begin).toInt();
    *sum = ans;
}

QDateTime UpdateData::to_datetime() {
    return QDateTime::fromString(time);
}

bool UpdateData::isBeta() {
    return _isBeta;
}
