#ifndef SERIALIZE_H
#define SERIALIZE_H
#include<QString>
#include<Base/Serializable.h>
#include<QDir>
#include<QJsonObject>
#include<QJsonDocument>
#include "debug.h"
class Serialize
{
public:
    static void serialize(QString path, Serializable* point)
    {
        QString dir_path = path.mid(0, path.lastIndexOf("/")+1);
        QDir base_dir = QDir(dir_path);
        if(!base_dir.exists())
        {
            QDir dir;
            dir.mkdir(dir_path);
        }
        QFile file(path);
        QJsonObject jsonObject = QJsonObject();
        try
        {
            file.open(QIODevice::ReadWrite | QIODevice::Text | QIODevice::Truncate);
        }
        catch (...)
        {
            Debug::show_error_message("配置文件打开错误");
            file.close();
        }
        point->serialized(&jsonObject);
        QJsonDocument jsonDoc;
        jsonDoc.setObject(jsonObject);
        file.write(jsonDoc.toJson());
        file.close();
    }

    static bool deserialize(QString path, Serializable* point)
    {
        QFile file(path);
        if(!file.exists())
        {
            return false;
        }
        else
        {
            try
            {
                file.open(QIODevice::ReadOnly | QIODevice::Text);
            } catch (...) {
                Debug::show_error_message("配置文件打开错误");
                file.close();
                return false;
            }

            QByteArray allData = file.readAll();
            file.close();

            QJsonParseError json_error;
               QJsonDocument jsonDoc(QJsonDocument::fromJson(allData, &json_error));
            if(json_error.error != QJsonParseError::NoError)
            {
                Debug::show_error_message("配置文件打开错误");
                return false;
            }

            QJsonObject rootObj = jsonDoc.object();
            point->deserialized(&rootObj);
        }
        return true;
    }
};

#endif // SERIALIZE_H
