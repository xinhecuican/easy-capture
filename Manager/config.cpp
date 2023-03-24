#include "config.h"
#include<QDir>
#include<QMessageBox>
#include<QJsonObject>
#include<QJsonDocument>
#include<QDomDocument>
#include<QDebug>
#include "Helper/Serialize.h"
#include "Helper/mstring.h"
#include <string>
#include "Helper/debug.h"
#include "update.h"

DEFINE_STRING(Config);

Config* Config::_instance = NULL;

Config::Config() {
    is_loading_translate = false;
    is_update_config = false;
    all_settings = QMap<int, QVariant>();
}

Config::~Config() {
    qDebug() << "config delete";
}

QString Config::readTranslate(int type) {
    if(!is_loading_translate) {
        //MString::load_from_file(":/Data/Languages/Config/");
//        MString::load_from_file("Data/Languages/Config/");
        is_loading_translate = true;
    }
    return MString::search("{" + eto_string((setting)type) + "}");
}

void Config::serialized(QJsonObject* json) {
    if(is_update_config) {
        QJsonValue value = QJsonValue::fromVariant(all_settings[update_setting]);
        if(json->contains(eto_string(update_setting)))
            (*json)[eto_string(update_setting)] = value;
        else
            json->insert(eto_string(update_setting), value);
    } else {
        for(setting i=capture_one_window; i<setting::COUNT; i = setting(i + 1)) {
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
    instance()->all_settings = temp_setting;
}

void Config::setConfig(setting type, QVariant data) {
    instance()->all_settings[type] = data;
}

void Config::setConfig(int type, QVariant data) {
    instance()->all_settings[type] = data;
}

QString Config::getConfigName(setting type) {
    return instance()->readTranslate(type);
}

QString Config::getConfigName(int type) {
    return instance()->readTranslate(type);
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

