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
    30,//history_num
    true,//rect_capture
    false,
    false,
    false,
    false,//need_update
    60 * 24 * 7,//update_interval
    false,
    false,
    false,
    true,
    0,
    false,//start_instantly
    true,
    true,//show_close_dialog
    true
};

DEFINE_STRING(Config);
QMap<int, int> Config::all_settings = QMap<int, int>();
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
        (*json)[eto_string(update_setting)] = all_settings[update_setting];
    }
    else
    {
        for(setting i=capture_one_window; i<setting::COUNT; i = setting(i + 1))
        {
            json->insert(eto_string((setting)i), all_settings[i]);
        }
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
    instance()->is_update_config = false;
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

void Config::update_config(setting type)
{
    instance()->update_setting = type;
    instance()->is_update_config = true;
    Serialize::append("Data/config.json", instance());
}

void Config::update_all()
{
    for(int i=0; i<COUNT; i++)
    {
        if(all_settings.find(i) == all_settings.end())
        {
            all_settings[i] = default_settings[i];
        }
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


