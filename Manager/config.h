#ifndef CONFIG_H
#define CONFIG_H
#include<QMap>
#include<QList>
#include "Base/Serializable.h"
#include "Helper/EnumReflect.h"

class Config : Serializable
{
public:
    MAKE_ENUM(setting,//使用宏解析，可以生成对应的字符串
              capture_window_num_begin,//begin
              capture_one_window = capture_window_num_begin,
              capture_multi_window_separate,
              capture_multi_window_combine,
              capture_window_num_end = capture_multi_window_combine,//end
              languages_begin,
              chinese = languages_begin,
              english,
              languages_end = english
              );
    Config();
    static Config*& instance()
    {
        if(_instance == NULL)
        {
            _instance = new Config();
        }
        return _instance;
    }
    void deserialized(QJsonObject* json) override;
    void serialized(QJsonObject* json) override;
    static void load_config();
    static void save_to_config();
    static int get_config(setting type);
    static int get_config(QString type);
    static int get_config(int type);
    static void set_config(setting type, int data);
    static void set_config(int type, int data);
    static QString get_config_name(setting type);
    static QString get_config_name(int type);
private:
    bool is_loading_translate = false;
    static Config* _instance;
    static QMap<int, int> all_settings;
    static const int default_settings[];
    QString read_translate(int type);
};


#endif // CONFIG_H
