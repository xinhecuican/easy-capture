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

DEFINE_STRING(Config);
QMap<int, QVariant> Config::all_settings = QMap<int, QVariant>();
Config* Config::_instance = NULL;

Config::Config()
{
    is_loading_translate = false;
    is_update_config = false;
}

QString Config::read_translate(int type)
{
    if(!is_loading_translate)
    {
        //MString::load_from_file(":/Data/Languages/Config/");
        MString::load_from_file("Data/Languages/Config/");
        is_loading_translate = true;
    }
    return MString::search("{" + eto_string((setting)type) + "}");
}

void Config::serialized(QJsonObject* json)
{
    if(is_update_config)
    {
        QJsonValue value = QJsonValue::fromVariant(all_settings[update_setting]);
        if(json->contains(eto_string(update_setting)))
            (*json)[eto_string(update_setting)] = value;
        else
            json->insert(eto_string(update_setting), value);
    }
    else
    {
        for(setting i=capture_one_window; i<setting::COUNT; i = setting(i + 1))
        {
            QJsonValue value = QJsonValue::fromVariant(all_settings[i]);
            json->insert(eto_string((setting)i), value);
        }
    }
}

void Config::deserialized(QJsonObject* json)
{
    for(int i=0; i<COUNT; i++)
    {
        all_settings[i] = (*json)[eto_string((setting)i)].toVariant();
    }
}

void Config::save_to_config()
{
    instance()->is_update_config = false;
    Serialize::serialize("Data/config.json", instance());
}

void Config::load_config()
{
    if(!Serialize::deserialize("Data/config.json", instance()))
    {
        if(!Serialize::deserialize(":/Data/default.json", instance()))
        {
            Debug::debug_print_warning("配置文件解析失败");
        }
        save_to_config();
    }
}

void Config::update_config(setting type)
{
    instance()->update_setting = type;
    instance()->is_update_config = true;
    Serialize::append("Data/config.json", instance());
}

void Config::update_all()
{
    QMap<int, QVariant> temp_setting;
    temp_setting = all_settings;
    if(!Serialize::deserialize(":/Data/default.json", instance()))
    {
        Debug::debug_print_warning("配置文件解析失败");
    }
    for(int i=0; i<COUNT; i++)
    {
        if(temp_setting.find(i) == temp_setting.end())
        {
            temp_setting[i] = all_settings[i];
            update_config((setting)i);
        }
    }
    all_settings = temp_setting;
}

void Config::setConfig(setting type, QVariant data)
{
    all_settings[type] = data;
}

void Config::setConfig(int type, QVariant data)
{
    all_settings[type] = data;
}

QString Config::get_config_name(setting type)
{
    return instance()->read_translate(type);
}

QString Config::get_config_name(int type)
{
    return instance()->read_translate(type);
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

