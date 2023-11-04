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
    void addBoolOption(TabWidget* tab, QString optionName, QString name, int index);
    void addComboOption(TabWidget* tab, QString optionName, QString text, QVector<QString> name, int index, bool isString=false);
    void addKeyOption(TabWidget* tab, int index, QString optionName, QString windowName, QString keyName);
    void addGlobalKeyOption(TabWidget* tab, int index, QString optionName, QString keyName);
    void addNumOption(TabWidget* tab, QString optionName, int index, QString name, int min, int max);
    void addFileOption(TabWidget* tab, QString optionName, QString name, int index);
    void addSpacer(TabWidget* tab, QString text = QString());
    void addText(TabWidget* tab, QString text, QString objectName);
    bool disableWindowClose() override;

public slots:
    void onClose(bool isSave);

private:
    Ui::Setting *ui;
    PList<TabWidget*> all_setting;
    void normal_settings();
    void capture_settings();
    void keySettings();
    void aboutTab();
    bool isChange=false;
};

#endif // SETTING_H
