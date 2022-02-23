#ifndef SETTING_H
#define SETTING_H

#include <QMainWindow>
#include "Panel/Widgets/tab_widget.h"
#include<QMap>
#include "Base/Window_base.h"
#include<QCloseEvent>
#include "Helper/plist.h"
#if _MSC_VER >= 1600
#pragma execution_character_set("utf-8")
#endif

namespace Ui {
class Setting;
}

class Setting : public Window_base
{
    Q_OBJECT

public:
    Q_INVOKABLE explicit Setting(QWidget *parent = nullptr);
    ~Setting();
    void closeEvent(QCloseEvent* event)override;
    void on_window_cancal() override;
    void on_window_select() override;

private:
    struct data
    {
        int type;
        int sum;
        data(int t, int s)
        {
            type = t;
            sum = s;
        }
        data()
        {
            type = 0;
            sum = false;
        }
    };
    Ui::Setting *ui;
    PList<Tab_widget*> all_setting;
    QList<data> ready_setting;
    void normal_settings();
    void capture_settings();
    void key_settings();
};

#endif // SETTING_H
