#include "config.h"
#include<QDir>
#include<QMessageBox>
#include<QJsonObject>
#include<QJsonDocument>
#include<QDebug>


const QString Config::bool_setting_name[] =
{
    "capture_one_window",
    "capture_multi_window_separate",//不同窗口之间不结合
    "capture_multi_window_combine",//多个窗口碰到自动结合
};

const bool Config::default_bool_settings[] =
{
    true,
    false,
    false
};

QMap<int, bool> Config::all_bool_settings = QMap<int, bool>();

Config::Config()
{

}

void Config::save_to_config()
{
    QDir base_dir = QDir("Data");
    if(!base_dir.exists())
    {
        QDir dir;
        dir.mkdir("Data");
    }
    QFile file(base_dir.path() + "/config.json");
    QJsonObject jsonObject;
    try
    {
        file.open(QIODevice::ReadWrite | QIODevice::Text | QIODevice::Truncate);
    }
    catch (...)
    {
        QMessageBox::critical(NULL, "错误", "配置文件打开错误",
                              QMessageBox::Yes, QMessageBox::Yes);
        file.close();
    }
    if(!file.exists())
    {
        for(int i=0; i<num_of_config; i++)
        {
            jsonObject.insert(bool_setting_name[i], default_bool_settings[i]);
        }
    }
    else
    {

        for(int i=0; i<num_of_config; i++)
        {
            jsonObject.insert(bool_setting_name[i], all_bool_settings[i]);
        }

    }
    QJsonDocument jsonDoc;
    jsonDoc.setObject(jsonObject);
    file.write(jsonDoc.toJson());
    file.close();

}

void Config::load_config()
{
    QFile file("Data/config.json");
    if(!file.exists())
    {
        for(int i=0; i<num_of_config; i++)
        {
            all_bool_settings[i] = default_bool_settings[i];
        }
        return;
    }
    else
    {
        try
        {
            file.open(QIODevice::ReadOnly | QIODevice::Text);
        } catch (...) {
            QMessageBox::critical(NULL, "错误", "配置文件打开错误",
                                  QMessageBox::Yes, QMessageBox::Yes);
            file.close();
        }

        QByteArray allData = file.readAll();
        file.close();

        QJsonParseError json_error;
           QJsonDocument jsonDoc(QJsonDocument::fromJson(allData, &json_error));
        if(json_error.error != QJsonParseError::NoError)
        {
            QMessageBox::critical(NULL, "错误", "配置文件打开错误",
                                  QMessageBox::Yes, QMessageBox::Yes);
            return;
        }

        QJsonObject rootObj = jsonDoc.object();
        for(int i=0; i<num_of_config; i++)
        {
            all_bool_settings[i] = rootObj[bool_setting_name[i]].toBool();
        }
    }
}

bool Config::get_config(setting type)
{
    return all_bool_settings[type];
}

bool Config::get_config(QString type)
{
    for(int i=0; i<bool_setting_name->size(); i++)
    {
        if(bool_setting_name[i] == type)
        {
            return all_bool_settings[i];
        }
    }
    return false;
}

bool Config::get_config(int type)
{
    return all_bool_settings[type];
}
void Config::set_config(setting type, bool data)
{
    all_bool_settings[type] = data;
}

void Config::set_config(int type, bool data)
{
    all_bool_settings[type] = data;
}

QString Config::get_config_name(setting type)
{
    return bool_setting_name[type];
}

QString Config::get_config_name(int type)
{
    return bool_setting_name[type];
}


