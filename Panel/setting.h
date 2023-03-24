#ifndef SETTING_H
#define SETTING_H

#include <QMainWindow>
#include "Panel/Widgets/tab_widget.h"
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

private:
    struct data {
        int type;
        QVariant sum;
        data(int t, QVariant s) {
            type = t;
            sum = s;
        }
        data() {
            type = 0;
            sum = false;
        }
    };
    Ui::Setting *ui;
    PList<Tab_widget*> all_setting;
    QList<data> ready_setting;
    void normal_settings();
    void capture_settings();
    void keySettings();
    void postUpdate(Config::setting type, QVariant num);
    Tab_widget* keySetting;
};

#endif // SETTING_H
