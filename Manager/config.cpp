#include "config.h"
#include<QDir>
#include<QMessageBox>
#include<QJsonObject>
#include<QJsonDocument>
#include<QDomDocument>
#include<QDebug>
#include "Helper/Serialize.h"
#include "Helper/mstring.h"

const int Config::default_settings[] =
{
    true,//capture_one_window
    false,
    false,
    true,//chinese
    false,
    30,//clear_interval,
    3,//history_num
    true,//rect_capture
    false,
    false,
    false,
    false
};

DEFINE_STRING(Config);

QMap<int, int> Config::all_settings = QMap<int, int>();
Config* Config::_instance = NULL;

Config::Config()
{
    is_loading_translate = false;
}

QString Config::read_translate(int type)
{
    if(!is_loading_translate)
    {
        MString::load_from_file(":/Data/Languages/Config/");
        is_loading_translate = true;
    }
    return MString::search("{" + eto_string((setting)type) + "}");
}

void Config::serialized(QJsonObject* json)
{
    for(setting i=capture_one_window; i<setting::COUNT; i = setting(i + 1))
    {
        json->insert(eto_string((setting)i), all_settings[i]);
    }
}

void Config::deserialized(QJsonObject* json)
{
    for(int i=0; i<COUNT; i++)
    {
        all_settings[i] = (*json)[eto_string((setting)i)].toInt();
    }
}

void Config::save_to_config()
{
    Serialize::serialize("Data/config.json", instance());
}

void Config::load_config()
{
    if(!Serialize::deserialize("Data/config.json", instance()))
    {
        for(int i=0; i<COUNT; i++)
        {
            all_settings[i] = default_settings[i];
        }
        save_to_config();
    }
}

int Config::get_config(setting type)
{
    return all_settings[type];
}

int Config::get_config(QString type)
{
    for(int i=0; i<COUNT; i++)
    {
        if(eto_string((setting)(i)) == type)
        {
            return all_settings[i];
        }
    }
    return 0;
}

int Config::get_config(int type)
{
    return all_settings[type];
}
void Config::set_config(setting type, int data)
{
    all_settings[type] = data;
}

void Config::set_config(int type, int data)
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


