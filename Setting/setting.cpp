#include "setting.h"
#include "ui_setting.h"

#include<QVector>
#include "Manager/config.h"
#include<QPushButton>
#include "Manager/WindowManager.h"
#include <QDebug>
#include <QDialogButtonBox>
#include "Manager/KeyManager.h"
#include<QLabel>
#include<QMessageBox>
#include<QSettings>
#include<QDir>
#include "Manager/update.h"
#include "Helper/Serialize.h"
#include "Manager/uimanager.h"

Setting::Setting(QWidget *parent) :
    WindowBase(parent),
    ui(new Ui::Setting) {
    ui->setupUi(this);
    all_setting = PList<TabWidget*>();

    QDialogButtonBox* buttonBox = new QDialogButtonBox(this);//右下角按钮设置
    QPushButton* resetButton = new QPushButton(this);
    resetButton->setText(MString::search("{4Rmr42SpQK}重置"));
    resetButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);
    connect(resetButton, &QPushButton::clicked, this, [=]() {
        Config::resetConfig();
        KeyManager::instance()->reset();
        for(int i=0; i<all_setting.size(); i++) {
            all_setting[i]->reset();
        }
        isChange = true;
    });
    QPushButton* ok = new QPushButton(this);
    ok->setText(MString::search("{H1WPwLFDwO}确认"));
    ok->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);
    QPushButton* close = new QPushButton(this);
    close->setText(MString::search("{FuBCvgW4BE}关闭"));
    close->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);
    buttonBox->addButton(resetButton, QDialogButtonBox::ResetRole);
    buttonBox->addButton(ok, QDialogButtonBox::AcceptRole);
    buttonBox->addButton(close, QDialogButtonBox::RejectRole);
    ok->connect(ok, &QPushButton::clicked, this, [=]() {
        onClose(true);
        this->close();
    });
    close->connect(close, &QPushButton::clicked, this, [=]() {
        bool isChange = this->isChange;
        if(!isChange){
            for(TabWidget* widget : all_setting){
                isChange = widget->isChange();
                if(isChange) break;
            }
        }
        if(isChange) {
            int ans = QMessageBox::warning(this, "简截", "有设置未保存，是否保存",  QMessageBox::No| QMessageBox::Ok);
            if(ans == QMessageBox::Ok) {
                onClose(true);
            }
            this->close();
        }
    });
    QVBoxLayout* base_layout = new QVBoxLayout();
    QVBoxLayout* card_layout = new QVBoxLayout();
    QHBoxLayout* button_layout = new QHBoxLayout();

    base_layout->addLayout(card_layout);
    base_layout->addLayout(button_layout);
    ui->centralwidget->setLayout(base_layout);
    button_layout->setDirection(QBoxLayout::RightToLeft);
    button_layout->addWidget(buttonBox);
    card_layout->addWidget(ui->tabWidget);
    normal_settings();
    capture_settings();
    keySettings();
    aboutTab();
}

Setting::~Setting() {
    delete ui;
}

TabWidget* Setting::addTab(QString name) {
    TabWidget* tab = new TabWidget(name, this);
    all_setting.append(tab);
    ui->tabWidget->addTab(tab, MString::search(name));
    return tab;
}

TabWidget* Setting::getTab(QString name) {
    for(TabWidget* tab : all_setting) {
        if(tab->getName() == name) {
            return tab;
        }
    }
    return NULL;
}

void Setting::addBoolOption(TabWidget* tab, QString tab_name, QString name, int index, const std::function<void (bool)> &f) {
    tab->add_bool_option(tab_name, name, index, f);

}

void Setting::addComboOption(TabWidget* tab, QString tab_name, QString text, QVector<QString> name, int index, const std::function<void (int)> &f, bool isString) {
    tab->add_combo_option(tab_name, text, name, index, f, isString);
}

void Setting::addKeyOption(TabWidget* tab, int index, QString indexName, QString windowName, QString keyName, std::function<void (QString, QString, QList<int>)> const &f) {
    tab->add_key_option(index, indexName, windowName, keyName, f);
}

void Setting::addGlobalKeyOption(TabWidget* tab, int index, QString optionName, QString keyName, std::function<void (QString, int, int)> const &f) {
    tab->addGlobalKeyOption(index, optionName, keyName, f);
}

void Setting::addNumOption(TabWidget* tab, QString optionName, int index, QString name, int min, int max, const std::function<void (int)> &f) {
    tab->add_num_option(optionName, index, name, min, max, f);
}

void Setting::addFileOption(TabWidget* tab, QString optionName, QString name, int index, const std::function<void (QString)> &f) {
    tab->add_file_option(optionName, name, index, f);
}

void Setting::addBoolOption(TabWidget* tab, QString tab_name, QString name, int index) {
    tab->addBoolOption(tab_name, name, index);

}

void Setting::addComboOption(TabWidget* tab, QString tab_name, QString text, QVector<QString> name, int index, bool isString) {
    tab->addComboOption(tab_name, text, name, index, isString);
}

void Setting::addKeyOption(TabWidget* tab, int index, QString indexName, QString windowName, QString keyName) {
    tab->addKeyOption(index, indexName, windowName, keyName);
}

void Setting::addGlobalKeyOption(TabWidget* tab, int index, QString optionName, QString keyName) {
    tab->addGlobalKeyOption(index, optionName, keyName);
}

void Setting::addNumOption(TabWidget* tab, QString optionName, int index, QString name, int min, int max) {
    tab->addNumOption(optionName, index, name, min, max);
}

void Setting::addFileOption(TabWidget* tab, QString optionName, QString name, int index) {
    tab->addFileOption(optionName, name, index);
}

void Setting::addSpacer(TabWidget* tab, QString text) {
    tab->add_spacer(text);
}

void Setting::addText(TabWidget* tab, QString text, QString objectName) {
    tab->addText(text, objectName);
}

void Setting::normal_settings() {
    TabWidget* normal_setting = addTab("{6m2deulC6q}通用");
    QVector<QString> update_name = QVector<QString>();
    update_name << MString::search("{ZmjIc1nu8Q}永不更新")
                << MString::search("{3EhZoQBZHg}每日检查更新")
                << MString::search("{F9JiJznbwk}每次启动检查更新")
                << MString::search("{4Xoj7GVwjR}每周检查更新");
    addComboOption(normal_setting, "update_type", "{1cSo8BaCHq}更新时间", update_name,
                   Config::update_checktime, [=](int index) {
                       Config::setConfig(Config::update_checktime, index);
                       Config::updateConfig(Config::update_checktime);
                       int sum;
                       switch(index) {
                       case 0:
                           sum = -1;
                           break;
                       case 1:
                           sum = 60 * 24;
                           break;
                       case 2:
                           sum = 0;
                           break;
                       case 3:
                           sum = 60 * 24 * 7;
                           break;
                       }
                       Config::setConfig(Config::update_interval, sum);
                       Config::updateConfig(Config::update_interval);
                   });

    QVector<QString> language_name = QVector<QString>();
    language_name << MString::search("{chinese}") << MString::search("{english}");
    addComboOption(normal_setting, "language_type", "{E33TP7yq9G}语言", language_name,
                   Config::language, [=](int index){
                       Config::setConfig(Config::language, index);
                       Config::updateConfig(Config::language);
                       MString::load_from_file("Data/Languages/");
                   });
    QVector<QString> themeName = QVector<QString>();
    QDir dir("Data/UI");
    if(dir.exists()) {
        QFileInfoList fileList = dir.entryInfoList(QDir::Dirs|QDir::NoDotAndDotDot);
        for(int i=0; i<fileList.size(); i++) {
            themeName.append(fileList.at(i).fileName());
        }
    }
    addComboOption(normal_setting, "theme_name", "{7BQgNdUtCI}主题", themeName, Config::ui_theme_name, [=](int index){
            Config::setConfig(Config::ui_theme_name, themeName[index]);
            Config::updateConfig(Config::ui_theme_name);
            delete UIManager::instance();
    }, true);
    addNumOption(normal_setting, "history_num", Config::history_num, "{VwuLzPpyI3}历史记录数量", 0, 500);
    addNumOption(normal_setting, "check_time", Config::clear_interval, "{dcvOkViWtX}内存清理检查时间", 5, 120);
    addBoolOption(normal_setting, "start_now", "{3BIidMbkAm}开机启动", Config::start_instantly, [=](bool ans) {
        Config::setConfig(Config::start_instantly, ans);
        Config::updateConfig(Config::start_instantly);
        QSettings reg("HKEY_CURRENT_USER\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run", QSettings::NativeFormat);
        QString appName = QApplication::applicationName();
        if(ans) {
            QString strAppPath=QDir::toNativeSeparators(QCoreApplication::applicationFilePath());
            reg.setValue(appName,strAppPath+ " autoStart");
        } else
            reg.remove(appName);
    });
    addBoolOption(normal_setting, "hide_tray", "{ZflcWUcIX8}隐藏到托盘", Config::hide_to_tray);
    addBoolOption(normal_setting, "copy_clipboard", "{g2X3jHOVbU}自动复制到剪切板", Config::auto_copy_to_clipboard);
    addBoolOption(normal_setting, "clip_voice", "{sqFHDuafTm}启用截屏声音", Config::clip_voice);
    addBoolOption(normal_setting, "receive_beta", "{PFu2ZARZg9}是否接受beta版本", Config::receive_beta);
}

void Setting::capture_settings() {
    TabWidget* capture_setting = addTab("{23ih0Dr8Na}捕获");

    QVector<QString> button_type_name = QVector<QString>();
    button_type_name << MString::search("{CcwwfzotQL}跳转到绘图窗口")
                     << MString::search("{Sx1TD3igtj}保存到剪切板")
                     << MString::search("{ePbIenISyt}保存到文件");
    addComboOption(capture_setting, "capture_button_type", "{PnHYo4i9Gu}鼠标中键作用", button_type_name, Config::middle_button_type);
    addNumOption(capture_setting, "capture_interval", Config::capture_interval, "{FcOnYo1uUa}滚动时间间隔", 50, 500);
    addNumOption(capture_setting, "scroll_end_times", Config::scroll_end_times, "{dOF2HHjdUt}滚动结束检测间隔", 1, 20);
    addFileOption(capture_setting, "global_capture_file", "{HBqaqm8LIK}全屏保存位置", Config::total_capture_save_path);
}

void Setting::keySettings() {
    TabWidget* keySetting = addTab("{YRHJ1nexv6}快捷键");
    addSpacer(keySetting, "{LvA0JggRsZ}全局快捷键");
    QList<QString> globalKeyNames = KeyManager::instance()->instance()->getGlobalKeyName();
    for(int i=0; i<globalKeyNames.size(); i++) {
        addGlobalKeyOption(keySetting, i, "{" + globalKeyNames[i] + "}" + globalKeyNames[i], globalKeyNames[i]);
    }

    QList<QString> window_name = KeyManager::instance()->instance()->getWindowNames();
    window_name.sort();
    for(int i=0; i<window_name.size(); i++) {
        QList<QString> key_name = KeyManager::instance()->instance()->getKeyNames(window_name[i]);
        key_name.sort();
        if(key_name.size() == 0) {
            continue;
        }
        addSpacer(keySetting, "{" + window_name[i] + "}" + window_name[i]);
        int size = key_name.size();
        for(int k=0; k<size; k++) {
            addKeyOption(keySetting, i * size + k, "{" + key_name[k] + "}" + key_name[k],
                         window_name[i], key_name[k]);
        }
    }
}

void Setting::aboutTab() {
    TabWidget* aboutTab = addTab("{ms01jwOir5}关于");
    aboutTab->addText("{YQQqFBaXjV}简截", "easyCapture");
    aboutTab->addText(MString::search("{SYdvb1dMci}版本") + ": " + Update::instance()->now_version.get_version(), "version");
    if(Update::instance()->now_version.get_time() == "") {
        Serialize::deserialize("Data/Temp/update_msg.json", Update::instance());
    }
    aboutTab->addText(MString::search("{UjoYcLzhiX}更新时间") + ": " + Update::instance()->now_version.get_time(), "updateTime");
    aboutTab->addText(MString::search("{R3hH9RwEIO}作者") + ": " + "星河璀璨", "author");

    QHBoxLayout* githubLayout = new QHBoxLayout();
    githubLayout->addWidget(new QLabel("github: ", aboutTab), 0);
    QLabel* linkLabel = new QLabel("<a href=\"https://github.com/xinhecuican/easy-capture\">https://github.com/xinhecuican/easy-capture", aboutTab);
    linkLabel->setOpenExternalLinks(true);
    linkLabel->setTextFormat(Qt::RichText);
    linkLabel->setTextInteractionFlags(Qt::TextBrowserInteraction);
    githubLayout->addWidget(linkLabel, 1);
    aboutTab->add_layout(githubLayout);

    aboutTab->addText("{0p65BvqadC}更新日志", "updateLog");
    aboutTab->addText(Update::instance()->now_version.get_description(), "");

}

void Setting::closeEvent(QCloseEvent *event) {
    WindowManager::instance()->checkWindow();
    event->accept();
}

void Setting::onWindowCancel() {
    //    ready_setting.clear();
    //    for(int i=0; i<all_setting.size(); i++) {
    //        if(all_setting[i]->is_dirty()) {
    //            all_setting[i]->reset();
    //        }
    //    }
}

void Setting::onWindowSelect() {
    ui->tabWidget->setCurrentIndex(0);
    update();
}

void Setting::onClose(bool isSave) {
    for(TabWidget* widget: all_setting){
        widget->onSave();
    }
}

bool Setting::disableWindowClose(){
    return true;
}
