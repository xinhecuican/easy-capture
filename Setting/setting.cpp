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
    ready_setting = QList<data>();
    all_setting = PList<TabWidget*>();

    QDialogButtonBox* buttonBox = new QDialogButtonBox(this);//右下角按钮设置
    QPushButton* resetButton = new QPushButton(this);
    resetButton->setText(MString::search("{4Rmr42SpQK}重置"));
    resetButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);
    connect(resetButton, &QPushButton::clicked, this, [=]() {
        TabWidget* keyTab = getTab("{YRHJ1nexv6}快捷键");
        if(keyTab != ui->tabWidget->currentWidget()) {
            Config::resetConfig();
            for(int i=0; i<all_setting.size(); i++) {
                if(all_setting[i] != keyTab)
                    all_setting[i]->reset();
            }
        } else {
            KeyManager::reset();
            keyTab->reset();
        }
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
        if(ready_setting.size() > 0) {
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

void Setting::postUpdate(Config::setting type, QVariant data) {
    switch(type) {
    case Config::language:
        MString::load_from_file("Data/Languages/");
        break;
    case Config::ui_theme_name:
        delete UIManager::instance();
        break;
    }
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

void Setting::addBoolOption(QString tabName, QString tab_name, QString name, int index, const std::function<void (bool)> &f) {
    TabWidget* panel = getTab(tabName);
    panel->add_bool_option(tab_name, name, index, f);

}

void Setting::addComboOption(QString tabName, QString tab_name, QString text, QVector<QString> name, int index, const std::function<void (int)> &f, bool isString) {
    TabWidget* panel = getTab(tabName);
    panel->add_combo_option(tab_name, text, name, index, f, isString);
}

void Setting::addKeyOption(QString tabName, int index, QString indexName, QString windowName, QString keyName, std::function<void (QString, QString, QList<int>)> const &f) {
    TabWidget* panel = getTab(tabName);
    panel->add_key_option(index, indexName, windowName, keyName, f);
}

void Setting::addGlobalKeyOption(QString tabName, int index, QString optionName, QString keyName, std::function<void (QString, int, int)> const &f) {
    TabWidget* tab = getTab(tabName);
    tab->addGlobalKeyOption(index, optionName, keyName, f);
}

void Setting::addNumOption(QString tabName, QString optionName, int index, QString name, int min, int max, const std::function<void (int)> &f) {
    TabWidget* tab = getTab(tabName);
    tab->add_num_option(optionName, index, name, min, max, f);
}

void Setting::addFileOption(QString tabName, QString optionName, QString name, int index, const std::function<void (QString)> &f) {
    TabWidget* tab = getTab(tabName);
    tab->add_file_option(optionName, name, index, f);
}

void Setting::addSpacer(QString tabName, QString text) {
    TabWidget* tab = getTab(tabName);
    tab->add_spacer(text);
}

void Setting::addText(QString tabName, QString text, QString objectName) {
    TabWidget* tab = getTab(tabName);
    tab->addText(text, objectName);
}

void Setting::normal_settings() {
    TabWidget* normal_setting = addTab("{6m2deulC6q}通用");
    QVector<QString> update_name = QVector<QString>();
    update_name << MString::search("{ZmjIc1nu8Q}永不更新")
                << MString::search("{3EhZoQBZHg}每日检查更新")
                << MString::search("{F9JiJznbwk}每次启动检查更新")
                << MString::search("{4Xoj7GVwjR}每周检查更新");
    addComboOption("{6m2deulC6q}通用", "update_type", "{1cSo8BaCHq}更新时间", update_name,
    Config::update_checktime, [=](int index) {
        ready_setting.append(data(Config::update_checktime, index));
        data temp_data;
        temp_data.type = Config::update_interval;
        switch(index) {
        case 0:
            temp_data.sum = -1;
            break;
        case 1:
            temp_data.sum = 60 * 24;
            break;
        case 2:
            temp_data.sum = 0;
            break;
        case 3:
            temp_data.sum = 60 * 24 * 7;
            break;
        }
        ready_setting.append(temp_data);
        normal_setting->set_dirty(true);
    });

    QVector<QString> language_name = QVector<QString>();
    language_name << MString::search("{chinese}") << MString::search("{english}");
    addComboOption("{6m2deulC6q}通用", "language_type", "{E33TP7yq9G}语言", language_name,
    Config::language, [=](int index) {
        ready_setting.append(data(Config::language, index));
        normal_setting->set_dirty(true);
    });
    QVector<QString> themeName = QVector<QString>();
    QDir dir("Data/UI");
    if(dir.exists()) {
        QFileInfoList fileList = dir.entryInfoList(QDir::Dirs|QDir::NoDotAndDotDot);
        for(int i=0; i<fileList.size(); i++) {
            themeName.append(fileList.at(i).fileName());
        }
    }
    addComboOption("{6m2deulC6q}通用", "theme_name", "{7BQgNdUtCI}主题", themeName, Config::ui_theme_name, [=](int index) {
        ready_setting.append(data(Config::ui_theme_name, themeName[index]));
        normal_setting->set_dirty(true);
    }, true);
    addNumOption("{6m2deulC6q}通用", "history_num", Config::history_num, "{VwuLzPpyI3}历史记录数量", 0, 500,[=](int index) {
        ready_setting.append(data(Config::history_num, index));
        normal_setting->set_dirty(true);
    });
    addNumOption("{6m2deulC6q}通用", "check_time", Config::clear_interval, "{dcvOkViWtX}内存清理检查时间", 5, 120, [=](int index) {
        ready_setting.append(data(Config::clear_interval, index));
        normal_setting->set_dirty(true);
    });
    addBoolOption("{6m2deulC6q}通用", "start_now", "{3BIidMbkAm}开机启动", Config::start_instantly, [=](bool ans) {
        ready_setting.append(data(Config::start_instantly, ans));
        QSettings reg("HKEY_CURRENT_USER\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run", QSettings::NativeFormat);
        QString appName = QApplication::applicationName();
        if(ans) {
            QString strAppPath=QDir::toNativeSeparators(QCoreApplication::applicationFilePath());
            reg.setValue(appName,strAppPath+ tr(" autoStart"));
        } else
            reg.remove(appName);
        normal_setting->set_dirty(true);
    });
    addBoolOption("{6m2deulC6q}通用", "hide_tray", "{ZflcWUcIX8}隐藏到托盘", Config::hide_to_tray, [=](bool ans) {
        ready_setting.append(data(Config::hide_to_tray, ans));
        normal_setting->set_dirty(true);
    });
    addBoolOption("{6m2deulC6q}通用", "copy_clipboard", "{g2X3jHOVbU}自动复制到剪切板", Config::auto_copy_to_clipboard, [=](bool ans) {
        ready_setting.append(data(Config::auto_copy_to_clipboard, ans));
        normal_setting->set_dirty(true);
    });
    addBoolOption("{6m2deulC6q}通用", "clip_voice", "{sqFHDuafTm}启用截屏声音", Config::clip_voice, [=](bool ans) {
        ready_setting.append(data(Config::clip_voice, ans));
        normal_setting->set_dirty(true);
    });
    addBoolOption("{6m2deulC6q}通用", "receive_beta", "{PFu2ZARZg9}是否接受beta版本", Config::receive_beta, [=](bool ans) {
        ready_setting.append(data(Config::receive_beta, ans));
        normal_setting->set_dirty(true);
    });
}

void Setting::capture_settings() {
    TabWidget* capture_setting = addTab("{23ih0Dr8Na}捕获");

    QVector<QString> button_type_name = QVector<QString>();
    button_type_name << MString::search("{CcwwfzotQL}跳转到绘图窗口")
                     << MString::search("{Sx1TD3igtj}保存到剪切板")
                     << MString::search("{ePbIenISyt}保存到文件");
    addComboOption("{23ih0Dr8Na}捕获", "capture_button_type", "{PnHYo4i9Gu}鼠标中键作用", button_type_name,
    Config::middle_button_type, [=](int index) {
        ready_setting.append(data(Config::middle_button_type, index));
        capture_setting->set_dirty(true);
    });
    addNumOption("{23ih0Dr8Na}捕获", "capture_interval", Config::capture_interval, "{FcOnYo1uUa}滚动时间间隔", 100, 500, [=](int index) {
        ready_setting.append(data(Config::capture_interval, index));
        capture_setting->set_dirty(true);
    });
    addFileOption("{23ih0Dr8Na}捕获", "global_capture_file", "{HBqaqm8LIK}全屏保存位置", Config::total_capture_save_path, [=](QString path) {
        ready_setting.append(data(Config::total_capture_save_path, path));
        capture_setting->set_dirty(true);
    });
}

void Setting::keySettings() {
    TabWidget* keySetting = addTab("{YRHJ1nexv6}快捷键");
    addSpacer("{YRHJ1nexv6}快捷键", "{LvA0JggRsZ}全局快捷键");
    QList<QString> globalKeyNames = KeyManager::getGlobalKeyName();
    for(int i=0; i<globalKeyNames.size(); i++) {
        addGlobalKeyOption("{YRHJ1nexv6}快捷键", i, "{" + globalKeyNames[i] + "}" + globalKeyNames[i], globalKeyNames[i], [=](QString keyName, int modKey, int key) {
            ready_setting.append(data(keyName, modKey, key));
            keySetting->set_dirty(true);
        });
    }

    QList<QString> window_name = KeyManager::getWindowNames();
    window_name.sort();
    for(int i=0; i<window_name.size(); i++) {
        QList<QString> key_name = KeyManager::getKeyNames(window_name[i]);
        key_name.sort();
        if(key_name.size() == 0) {
            continue;
        }
        addSpacer("{YRHJ1nexv6}快捷键", "{" + window_name[i] + "}" + window_name[i]);
        int size = key_name.size();
        for(int k=0; k<size; k++) {
            addKeyOption("{YRHJ1nexv6}快捷键", i * size + k, "{" + key_name[k] + "}" + key_name[k],
            window_name[i], key_name[k], [=](QString windowName, QString keyName, QList<int> keys) {
                ready_setting.append(data(windowName, keyName, keys));
                keySetting->set_dirty(true);
            });
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
    WindowManager::popWindow();
    WindowManager::closeWindow("Setting");
    event->accept();
}

void Setting::onWindowCancel() {
//    ready_setting.clear();
//    for(int i=0; i<all_setting.size(); i++) {
//        if(all_setting[i]->is_dirty()) {
//            all_setting[i]->reset();
//        }
//    }
    KeyManager::registerAll();
}

void Setting::onWindowSelect() {
    ui->tabWidget->setCurrentIndex(0);
    KeyManager::unRegisterAll();
    update();
}

void Setting::onClose(bool isSave) {
    if(isSave) {
        for(int i=0; i<ready_setting.size(); i++) {
            if(ready_setting[i].location == Config)
                Config::setConfig(ready_setting[i].type, ready_setting[i].sum);
            else if(ready_setting[i].location == Key)
                KeyManager::setKeys(ready_setting[i].windowName, ready_setting[i].keyName, ready_setting[i].keys);
            else if(ready_setting[i].location == GlobalKey)
                KeyManager::addGlobalKey(ready_setting[i].keyName, ready_setting[i].modKey, ready_setting[i].key);
        }
        Config::saveToConfig();
        KeyManager::save();
        for(TabWidget* tab : all_setting) {
            tab->set_dirty(false);
        }
    }
}
