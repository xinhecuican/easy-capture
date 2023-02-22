#include "setting.h"
#include "ui_setting.h"

#include<QVector>
#include "Manager/config.h"
#include<QPushButton>
#include "Manager/window_manager.h"
#include <QDebug>
#include <QDialogButtonBox>
#include "Manager/key_manager.h"
#include<QLabel>
#include<QMessageBox>
#include<QSettings>
#include<QDir>

Setting::Setting(QWidget *parent) :
    Window_base(parent, this, "Setting"),
    ui(new Ui::Setting)
{
    ui->setupUi(this);
    ready_setting = QList<data>();
    all_setting = PList<Tab_widget*>();

    QDialogButtonBox* buttonBox = new QDialogButtonBox(this);//右下角按钮设置
    QPushButton* ok = new QPushButton(this);
    ok->setText("确认");
    ok->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);
    QPushButton* close = new QPushButton(this);
    close->setText("关闭");
    close->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);
    buttonBox->addButton(ok, QDialogButtonBox::AcceptRole);
    buttonBox->addButton(close, QDialogButtonBox::RejectRole);
    ok->connect(ok, &QPushButton::clicked, this, [=](){
        for(int i=0; i<ready_setting.size(); i++)
        {
            Config::setConfig(ready_setting[i].type, ready_setting[i].sum);
        }
        Config::save_to_config();
        Key_manager::save();
        Window_manager::pop_window();
    });
    close->connect(close, &QPushButton::clicked, this, [=](){
        if(ready_setting.size() > 0)
        {
            int ans = QMessageBox::warning(this, "简截", "有设置未保存，是否保存",
                                 QMessageBox::No| QMessageBox::Ok);
            if(ans == QMessageBox::Ok)
            {
                for(int i=0; i<ready_setting.size(); i++)
                {
                    Config::setConfig(ready_setting[i].type, ready_setting[i].sum);
                    postUpdate((Config::setting)ready_setting[i].type, ready_setting[i].sum);
                }
                Config::save_to_config();
                Key_manager::save();
            }
            else{
                key_setting->restore();
            }
        }
        Window_manager::pop_window();
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
    key_settings();
}

Setting::~Setting()
{
    delete ui;
}

void Setting::postUpdate(Config::setting type, QVariant data)
{
    switch(type)
    {
    case Config::chinese:
    case Config::english:
        MString::load_from_file("Data/Languages/");
        break;
    }
}

void Setting::normal_settings()
{
    Tab_widget* normal_setting = new Tab_widget("通用", this);
    QVector<QString> update_name = QVector<QString>();
    for(int i=Config::update_checktime_begin; i<=Config::update_checktime_end; i++)
    {
        update_name.push_back(Config::get_config_name(Config::setting(i)));
    }
    normal_setting->add_combo_option("update_type", "{1cSo8BaCHq}更新时间", update_name,
                                     Config::update_checktime_begin, Config::update_checktime_end, [=](int index){
        int before_index = normal_setting->get_default_index("update_type");
        ready_setting.append(data(before_index, false));
        ready_setting.append(data(index+normal_setting->get_begin_index("update_type"), true));
        data temp_data;
        temp_data.type = Config::update_interval;
        switch(index)
        {
        case 0:temp_data.sum = -1;break;
        case 1:temp_data.sum = 60 * 24;break;
        case 2:temp_data.sum = 0;break;
        case 3:temp_data.sum = 60 * 24 * 7;break;
        }
        ready_setting.append(temp_data);
        normal_setting->set_dirty(true);
    });
    QVector<QString> language_name = QVector<QString>();
    for(int i=Config::languages_begin; i<=Config::languages_end; i++)
    {
        language_name.push_back(Config::get_config_name(Config::setting(i)));
    }
    normal_setting->add_combo_option("language_type", "{E33TP7yq9G}语言", language_name,
                                     Config::languages_begin, Config::languages_end, [=](int index){
        int before_index = normal_setting->get_default_index("language_type");
        ready_setting.append(data(before_index, false));
        ready_setting.append(data(index+normal_setting->get_begin_index("language_type"), true));
        normal_setting->set_dirty(true);
    });
    normal_setting->add_num_option("history_num", Config::history_num, "{VwuLzPpyI3}历史记录数量", 0, 500,[=](int index){
        ready_setting.append(data(Config::history_num, index));
        normal_setting->set_dirty(true);
    });
    normal_setting->add_num_option("check_time", Config::clear_interval, "{dcvOkViWtX}内存清理检查时间", 5, 120, [=](int index){
        ready_setting.append(data(Config::clear_interval, index));
        normal_setting->set_dirty(true);
    });
    normal_setting->add_bool_option("start_now", "{3BIidMbkAm}开机启动", Config::start_instantly, [=](bool ans){
        ready_setting.append(data(Config::start_instantly, ans));
        QSettings reg("HKEY_CURRENT_USER\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run", QSettings::NativeFormat);
        QString appName = QApplication::applicationName();
        if(ans)
        {
            QString strAppPath=QDir::toNativeSeparators(QCoreApplication::applicationFilePath());
            reg.setValue(appName,strAppPath+ tr(" autoStart"));
        }
        else
            reg.remove(appName);
        normal_setting->set_dirty(true);
    });
    normal_setting->add_bool_option("hide_tray", "{ZflcWUcIX8}隐藏到托盘", Config::hide_to_tray, [=](bool ans){
        ready_setting.append(data(Config::hide_to_tray, ans));
        normal_setting->set_dirty(true);
    });
//    normal_setting->add_bool_option("show_close", "{YycWGmFDhU}显示关闭对话框", Config::show_close_dialog, [=](bool ans){
//        ready_setting.append(data(Config::show_close_dialog, ans));
//        normal_setting->set_dirty(true);
//    });
    normal_setting->add_bool_option("copy_clipboard", "{g2X3jHOVbU}自动复制到剪切板", Config::auto_copy_to_clipboard, [=](bool ans){
        ready_setting.append(data(Config::auto_copy_to_clipboard, ans));
        normal_setting->set_dirty(true);
    });
    normal_setting->add_bool_option("clip_voice", "{sqFHDuafTm}启用截屏声音", Config::clip_voice, [=](bool ans){
        ready_setting.append(data(Config::clip_voice, ans));
        normal_setting->set_dirty(true);
    });
    normal_setting->done();
    all_setting.append(normal_setting);
    ui->tabWidget->addTab(normal_setting, MString::search("{6m2deulC6q}通用"));
}

void Setting::capture_settings()
{
    Tab_widget* capture_setting = new Tab_widget("捕获", this);
    QVector<QString> button_type_name = QVector<QString>();
    button_type_name << MString::search("{CcwwfzotQL}跳转到绘图窗口")
                     << MString::search("{Sx1TD3igtj}保存到剪切板")
                     << MString::search("{ePbIenISyt}保存到文件");
    capture_setting->add_combo_option("capture_button_type", "{PnHYo4i9Gu}鼠标中键作用", button_type_name,
                                      Config::middle_button_type, Config::middle_button_type, [=](int index){
        ready_setting.append(data(Config::middle_button_type, index));
        capture_setting->set_dirty(true);
    });
//    capture_setting->add_combo_option("capture_type", "{biSbDKXbuW}捕获窗口数量设置", capture_type_name,
//                                      Config::capture_window_num_begin, Config::capture_window_num_end, [=](int index){
//        int before_index = capture_setting->get_default_index("capture_type");
//        ready_setting.append(data(before_index, false));
//        ready_setting.append(data(index+capture_setting->get_begin_index("capture_type"), true));
//        capture_setting->set_dirty(true);
//    });
    capture_setting->add_num_option("capture_interval", Config::capture_interval, "{FcOnYo1uUa}滚动时间间隔", 100, 500, [=](int index){
        ready_setting.append(data(Config::capture_interval, index));
        capture_setting->set_dirty(true);
    });
    capture_setting->add_file_option("global_capture_file", "{HBqaqm8LIK}全屏保存位置", Config::total_capture_save_path, [=](QString path){
        ready_setting.append(data(Config::total_capture_save_path, path));
        capture_setting->set_dirty(true);
    });
    capture_setting->done();
    all_setting.append(capture_setting);
    ui->tabWidget->addTab(capture_setting, MString::search("{23ih0Dr8Na}捕获"));
}

void Setting::key_settings()
{
    key_setting = new Tab_widget("快捷键", this);
    key_setting->add_spacer("{LvA0JggRsZ}全局快捷键");
//    QHBoxLayout* hlayout = new QHBoxLayout();
//    QLabel* label1 = new QLabel(MString::search("{iy3cIujuw5}打开截屏窗口"), key_setting);
//    QLabel* label2 = new QLabel("Ctrl+F1", key_setting);
//    QHBoxLayout* hlayout2 = new QHBoxLayout();
//    QLabel* label3 = new QLabel(MString::search("{0NHvAJ2fEQ}截取全屏"), key_setting);
//    QLabel* label4 = new QLabel("Ctrl+F2", key_setting);
//    hlayout->addWidget(label1);
//    hlayout->addWidget(label2);
//    hlayout2->addWidget(label3);
//    hlayout2->addWidget(label4);
//    key_setting->add_layout(hlayout);
//    key_setting->add_layout(hlayout2);
    QList<QString> globalKeyNames = Key_manager::getGlobalKeyName();
    for(int i=0; i<globalKeyNames.size(); i++){
        key_setting->addGlobalKeyOption(i, "{" + globalKeyNames[i] + "}" + globalKeyNames[i], globalKeyNames[i]);
    }

    QList<QString> window_name = Key_manager::get_window_names();
    window_name.sort();
    for(int i=0; i<window_name.size(); i++)
    {
        QList<QString> key_name = Key_manager::get_key_names(window_name[i]);
        key_name.sort();
        key_setting->add_spacer("{" + window_name[i] + "}" + window_name[i]);
        int size = key_name.size();
        for(int k=0; k<size; k++)
        {
            key_setting->add_key_option(i * size + k, "{" + key_name[k] + "}" + key_name[k],
                                        window_name[i], key_name[k]);
        }
    }
    key_setting->done();
    all_setting.append(key_setting);
    ui->tabWidget->addTab(key_setting, MString::search("{YRHJ1nexv6}快捷键"));
}

void Setting::closeEvent(QCloseEvent *event)
{
    key_setting->restore();
    Window_manager::pop_window();
    event->accept();
}

void Setting::on_window_cancal()
{
    ready_setting.clear();
    for(int i=0; i<all_setting.size(); i++)
    {
        if(all_setting[i]->is_dirty())
        {
            all_setting[i]->reset();
        }
    }
    Key_manager::registerAll();
}

void Setting::on_window_select()
{
    ui->tabWidget->setCurrentIndex(0);
    Key_manager::unRegisterAll();
    update();
}
