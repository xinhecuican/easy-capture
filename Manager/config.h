#ifndef CONFIG_H
#define CONFIG_H
#include<QMap>
#include<QList>


class Config
{
public:
    enum setting
    {
        capture_one_window,
        capture_multi_window_separate,
        capture_multi_window_combine,
        num_of_config
    };
    Config();
    static void load_config();
    static void save_to_config();
    static bool get_config(setting type);
    static bool get_config(QString type);
    static bool get_config(int type);
    static void set_config(setting type, bool data);
    static void set_config(int type, bool data);
    static QString get_config_name(setting type);
    static QString get_config_name(int type);
private:
    static const QString bool_setting_name[];
    static QMap<int, bool> all_bool_settings;
    static const bool default_bool_settings[];
};

#endif // CONFIG_H
