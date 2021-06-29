#include "setting.h"
#include "ui_setting.h"

#include<QVector>
#include "Manager/config.h"
#include<QPushButton>
#include "Manager/window_manager.h"
#include <QDebug>
#include <QDialogButtonBox>

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
            Config::set_config(ready_setting[i].type, ready_setting[i].sum);
        }
        Config::save_to_config();
        Window_manager::pop_window();
    });
    close->connect(close, &QPushButton::clicked, this, [=](){
        Window_manager::pop_window();
    });
    QVBoxLayout* base_layout = new QVBoxLayout(this);
    QVBoxLayout* card_layout = new QVBoxLayout(this);
    QHBoxLayout* button_layout = new QHBoxLayout(this);

    base_layout->addLayout(card_layout);
    base_layout->addLayout(button_layout);
    ui->centralwidget->setLayout(base_layout);
    button_layout->setDirection(QBoxLayout::RightToLeft);
    button_layout->addWidget(buttonBox);
    card_layout->addWidget(ui->tabWidget);

    Tab_widget* capture_setting = new Tab_widget("捕获", this);
    QVector<QString> capture_type_name = QVector<QString>();
    capture_type_name << Config::get_config_name(Config::capture_one_window)
                      << Config::get_config_name(Config::capture_multi_window_separate)
                      << Config::get_config_name(Config::capture_multi_window_combine);
    capture_setting->add_combo_option("capture_type", "{biSbDKXbuW}捕获窗口数量设置", capture_type_name,
                                      Config::capture_window_num_begin, Config::capture_window_num_end, [=](int index){
        int before_index = capture_setting->get_default_index("capture_type");
        ready_setting.append(data(before_index, false));
        ready_setting.append(data(index+capture_setting->get_begin_index("capture_type"), true));
        capture_setting->set_dirty(true);
    });
    all_setting.append(capture_setting);
    ui->tabWidget->addTab(capture_setting, MString::search("{23ih0Dr8Na}捕获"));
}

Setting::~Setting()
{
    delete ui;
}

void Setting::closeEvent(QCloseEvent *event)
{
    Window_manager::pop_window();
    event->ignore();
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
}

void Setting::on_window_select()
{
    ui->tabWidget->setCurrentIndex(0);
    update();
}
