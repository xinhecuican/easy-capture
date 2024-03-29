#include "config.h"
#include<QDir>
#include<QMessageBox>
#include<QJsonObject>
#include<QJsonDocument>
#include<QDomDocument>
#include<QDebug>
#include "../Helper/Serialize.h"
#include "../Helper/mstring.h"
#include <string>
#include "../Helper/debug.h"
#include "update.h"

DEFINE_STRING(Config);

std::atomic<Config*> Config::_instance = {nullptr};
std::mutex Config::mutex;

Config::Config() {
    is_loading_translate = false;
    is_update_config = false;
    all_settings = QMap<int, QVariant>();
}

Config::~Config() {
    qDebug() << "config delete";
}

void Config::serialized(QJsonObject* json) {
    if(is_update_config) {
        QJsonValue value = QJsonValue::fromVariant(all_settings[update_setting]);
        if(json->contains(eto_string(update_setting)))
            (*json)[eto_string(update_setting)] = value;
        else
            json->insert(eto_string(update_setting), value);
    } else {
        for(setting i=language; i<setting::COUNT; i = setting(i + 1)) {
            QJsonValue value = QJsonValue::fromVariant(all_settings[i]);
            json->insert(eto_string((setting)i), value);
        }
    }
}

void Config::deserialized(QJsonObject* json) {
    for(int i=0; i<COUNT; i++) {
        if((*json).find(eto_string((setting)i)) != (*json).end())
            all_settings[i] = (*json)[eto_string((setting)i)].toVariant();
    }
}

void Config::saveToConfig() {
    instance()->is_update_config = false;
    Serialize::serialize("Data/config.json", instance());
}

void Config::loadConfig() {
    if(!Serialize::deserialize("Data/config.json", instance())) {
        if(!Serialize::deserialize(":/Data/default.json", instance())) {
            qWarning("配置文件解析失败");
        }
        saveToConfig();
    }
    if(getConfig<QString>(version) != Update::now_version.get_version()) {
        updateAll();
    }
}

void Config::resetConfig() {
    if(!Serialize::deserialize(":/Data/default.json", instance())) {
        qWarning("配置文件解析失败");
    }
}

void Config::updateConfig(setting type) {
    instance()->update_setting = type;
    instance()->is_update_config = true;
    Serialize::append("Data/config.json", instance());
    instance()->is_update_config = false;
}

void Config::updateAll() {
    QMap<int, QVariant> temp_setting;
    temp_setting = instance()->all_settings;
    instance()->all_settings = QMap<int, QVariant>();
    if(!Serialize::deserialize(":/Data/default.json", instance())) {
        qWarning("配置文件解析失败");
    }
    for(int i=0; i<COUNT; i++) {
        if(temp_setting.find(i) == temp_setting.end()) {
            temp_setting[i] = instance()->all_settings[i];
            updateConfig((setting)i);
        }
    }
    temp_setting[version] = instance()->all_settings[version];
    instance()->all_settings = temp_setting;
    saveToConfig();
}

void Config::setConfig(setting type, QVariant data) {
    instance()->all_settings[type] = data;
}

void Config::setConfig(int type, QVariant data) {
    instance()->all_settings[type] = data;
}

//template<typename T>
//T Config::getConfig(setting type)
//{
//    QVariant var;
//    if(var.canConvert<T>())
//    {
//        return all_settings[type].value<T>();
//    }
//    return T();
//}

//template<typename T>
//T Config::getConfig(int type)
//{
//    QVariant var;
//    if(var.canConvert<T>())
//    {
//        return all_settings[type].value<T>();
//    }
//    return T();
//}

