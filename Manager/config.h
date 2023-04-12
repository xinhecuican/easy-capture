#ifndef CONFIG_H
#define CONFIG_H
#include<QMap>
#include<QList>
#include "Base/Serializable.h"
#include "Helper/EnumReflect.h"
#include <QJsonValue>
#include <atomic>
#include <mutex>

class Config : Serializable {
public:
    MAKE_ENUM(setting,//使用宏解析，可以生成对应的字符串
              language,
              clear_interval,
              history_num,
              capture_mode, ///< CaptureMode
              need_update,/// 需要更新
              update_interval,/// 更新检查时间间隔，-1表示永不更新
              update_checktime, ///< UpdateCheckTime
              last_update_time,/// 上次更新时间,注意以分钟为单位（秒为单位太小了，会超界）
              start_instantly,/// 开机启动
              hide_to_tray,
              show_close_dialog,
              auto_copy_to_clipboard,/// 自动复制到剪切板
              capture_interval,/// 滚动截屏时间间隔
              total_capture_save_path,
              middle_button_type,
              version,
              clip_voice,
              show_update_box,
              ui_theme_name,
              receive_beta
             );

    /**
     * @brief 截图模式
     */
    enum CaptureMode {
        RECT_CAPTURE, /// 普通模式
        TOTAL_CAPTURE, /// 截全屏
        FREE_CAPTURE, /// 自由形状截图
        SCROLL_CAPTURE /// 滚动截屏
    };

    /**
     * @brief 更新模式
     */
    enum UpdateCheckTime {
        NEVER_UPDATE, /// 永不更新
        UPDATE_DAY, /// 每日检查一次更新
        UPDATE_EVERYTIME, /// 每次启动检查更新
        UPDATE_WEEK /// 每周检查一次更新
    };

    enum Language {
        CHINESE,
        ENGLISH,
        LANGUAGE_END
    };

    Config();
    ~Config();
    static Config* instance() {
        Config* p = _instance;
        if (p == nullptr) {
            std::lock_guard<std::mutex> lock{ mutex };
            if ((p = _instance) == nullptr) {
                _instance = p = new Config();
            }
        }
        return p;
    }
    void deserialized(QJsonObject* json) override;
    void serialized(QJsonObject* json) override;
    static void loadConfig();
    static void saveToConfig();
    static void resetConfig();
    static void updateConfig(setting type);
    static void updateAll();

    template<typename T>
    /**
     * @brief getConfig 获得设置，模板T是变量类型
     * @param type
     * @return
     */
    static T getConfig(setting type) {
        if(instance()->all_settings.find(type) == instance()->all_settings.end())
            return T();
        if(instance()->all_settings[type].canConvert<T>()) {
            return instance()->all_settings[type].value<T>();
        }
        return T();

    }
    template<typename T>
    static T getConfig(int type) {
        if(instance()->all_settings.find(type) == instance()->all_settings.end())
            return T();
        if(instance()->all_settings[type].canConvert<T>()) {
            return instance()->all_settings[type].value<T>();
        }
        return T();
    }
    static void setConfig(setting type, QVariant data);
    static void setConfig(int type, QVariant data);
    static QString getConfigName(setting type);
    static QString getConfigName(int type);
private:
    bool is_loading_translate = false;
    static std::atomic<Config*> _instance;
    static std::mutex mutex;
    QMap<int, QVariant> all_settings;
    bool is_update_config;
    setting update_setting;
    QString readTranslate(int type);
};


#endif // CONFIG_H
