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
private:
    static const QString setting_name[];
    static QMap<int, bool> all_settings;
    static const bool default_settings[];
};

#endif // CONFIG_H
