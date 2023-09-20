#ifndef SETTING_H
#define SETTING_H

#include <QMainWindow>
#include "Setting/Widgets/TabWidget.h"
#include<QMap>
#include "Base/WindowBase.h"
#include<QCloseEvent>
#include "Helper/plist.h"
#include "Manager/config.h"

namespace Ui {
class Setting;
}

class Setting : public WindowBase {
    Q_OBJECT

public:
    Q_INVOKABLE explicit Setting(QWidget *parent = nullptr);
    ~Setting();
    void closeEvent(QCloseEvent* event)override;
    void onWindowCancel() override;
    void onWindowSelect() override;
    TabWidget* addTab(QString name);
    TabWidget* getTab(QString name);
    void addBoolOption(TabWidget* tab, QString optionName, QString name, int index, std::function<void (bool)> const  &f);
    void addComboOption(TabWidget* tab, QString optionName, QString text, QVector<QString> name,
                        int index, std::function<void (int)> const &f, bool isString=false);
    void addKeyOption(TabWidget* tab, int index, QString optionName, QString windowName, QString keyName, std::function<void (QString, QString, QList<int>)> const &f);
    void addGlobalKeyOption(TabWidget* tab, int index, QString optionName, QString keyName, std::function<void (QString, int, int)> const &f);
    void addNumOption(TabWidget* tab, QString optionName, int index, QString name, int min, int max,
                      std::function<void(int)> const &f);
    void addFileOption(TabWidget* tab, QString optionName, QString name, int index, std::function<void(QString)> const &f);
    void addSpacer(TabWidget* tab, QString text = QString());
    void addText(TabWidget* tab, QString text, QString objectName);
    bool disableWindowClose() override;

public slots:
    void onClose(bool isSave);

private:
    enum Location {Config, Key, GlobalKey};
    struct data {
        Location location;
        union {
            int type;
            int modKey;
        };
        int key;
        QVariant sum;
        QString windowName;
        QString keyName;
        QList<int> keys;
        data(int t, QVariant s) {
            location = Config;
            type = t;
            sum = s;
        }

        data(QString windowName, QString keyName, QList<int> keys) {
            location = Key;
            this->windowName = windowName;
            this->keyName = keyName;
            this->keys = keys;
        }

        data(QString keyName, int modKey, int key) {
            location = GlobalKey;
            this->keyName = keyName;
            this->modKey = modKey;
            this->key = key;
        }

        data() {
            location = Config;
            type = 0;
            sum = false;
        }
    };
    Ui::Setting *ui;
    PList<TabWidget*> all_setting;
    QList<data> ready_setting;
    void normal_settings();
    void capture_settings();
    void keySettings();
    void aboutTab();
    void postUpdate(Config::setting type, QVariant num);
};

#endif // SETTING_H
