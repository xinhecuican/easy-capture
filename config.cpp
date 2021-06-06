#include "config.h"
#include<QDir>
#include<QMessageBox>
#include<QJsonObject>
#include<QJsonDocument>
#include<QDebug>


const QString Config::setting_name[] =
{
    "capture_one_window",
    "capture_multi_window_separate",//不同窗口之间不结合
    "capture_multi_window_combine"//多个窗口碰到自动结合
};

const bool Config::default_settings[] =
{
    true,
    false,
    false
};

QMap<int, bool> Config::all_settings = QMap<int, bool>();

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
        file.open(QIODevice::ReadWrite);
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
            jsonObject.insert(setting_name[i], default_settings[i]);
        }
    }
    else
    {

        for(int i=0; i<num_of_config; i++)
        {
            jsonObject.insert(setting_name[i], all_settings[i]);
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
            all_settings[i] = default_settings[i];
        }
        return;
    }
    else
    {
        try
        {
            file.open(QIODevice::ReadOnly);
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
            all_settings[i] = rootObj[setting_name[i]].toBool();
        }
    }
}

bool Config::get_config(setting type)
{
    return all_settings[type];
}


