#ifndef CONFIG_H
#define CONFIG_H
#include<QMap>
#include<QList>
#include "Base/Serializable.h"
#include "Helper/EnumReflect.h"
#include <QJsonValue>

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
              languages_end = english,
              clear_interval,
              history_num,
              capture_mode_begin,
              rect_capture = capture_mode_begin,
              total_capture,//在Capture_button_action中
              free_capture,//在Capture_window中
              scroll_capture,
              capture_mode_end = scroll_capture,
              need_update,//需要更新
              update_interval,//更新检查时间间隔，-1表示永不更新
              update_checktime_begin,
              never_update = update_checktime_begin,
              update_day,
              update_everytime,
              update_week,
              update_checktime_end = update_week,
              last_update_time,//上次更新时间,注意以分钟为单位（秒为单位太小了，会超界）
              start_instantly,//开机启动
              hide_to_tray,
              show_close_dialog,
              auto_copy_to_clipboard,//自动复制到剪切板
              capture_interval,//滚动截屏时间间隔
              total_capture_save_path,
              middle_button_type,
              version,
              clip_voice,
              show_update_box
              );
    Config();
    ~Config();
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
    static void update_config(setting type);
    static void update_all();
    template<typename T>
    static T getConfig(setting type)
    {
        if(instance()->all_settings.find(type) == instance()->all_settings.end())
            return T();
        if(instance()->all_settings[type].canConvert<T>())
        {
            return instance()->all_settings[type].value<T>();
        }
        return T();

    }
    template<typename T>
    static T getConfig(int type)
    {
        if(instance()->all_settings.find(type) == instance()->all_settings.end())
            return T();
        if(instance()->all_settings[type].canConvert<T>())
        {
            return instance()->all_settings[type].value<T>();
        }
        return T();
    }
    static void setConfig(setting type, QVariant data);
    static void setConfig(int type, QVariant data);
    static QString get_config_name(setting type);
    static QString get_config_name(int type);
private:
    bool is_loading_translate = false;
    static Config* _instance;
    QMap<int, QVariant> all_settings;
    bool is_update_config;
    setting update_setting;
    QString read_translate(int type);
};


#endif // CONFIG_H
