#include "close_dialog.h"
#include<QLayout>
#include<QLabel>
#include<QRadioButton>
#include<QDialogButtonBox>
#include<QPushButton>
#include "Manager/WindowManager.h"
#include<QMessageBox>
#include<QFileDialog>
#include "Paint/Widgets/history.h"
#include<QButtonGroup>
#include "Manager/config.h"

Close_dialog::Close_dialog()
{
    is_close = true;
}

Close_dialog::Close_dialog(PaintArea* area, QWidget* parent) : QDialog(parent)
{
    setAttribute(Qt::WA_DeleteOnClose);
    QVBoxLayout* layout = new QVBoxLayout(this);
    QLabel* warning_icon = new QLabel(this);
    warning_icon->setPixmap(QPixmap(":/image/warning.png"));
    QGridLayout* grid_layout = new QGridLayout();

    QButtonGroup* group = new QButtonGroup(this);
    QRadioButton* hide_button = new QRadioButton("缩小到系统托盘", this);
    hide_button->setCheckable(true);
    QRadioButton* close_button = new QRadioButton("直接关闭", this);
    close_button->setCheckable(true);
    QHBoxLayout* hide_layout = new QHBoxLayout();
    group->addButton(hide_button, 0);
    group->addButton(close_button, 1);
    group->setExclusive(true);
    if(Config::getConfig<bool>(Config::hide_to_tray))
    {
        hide_button->setChecked(true);
    }
    else
    {
        close_button->setChecked(true);
    }
    connect(group, static_cast<void(QButtonGroup::*)(int)>(&QButtonGroup::buttonClicked), this, [=](int id){
        if(id == 0)
        {
            Config::setConfig(Config::hide_to_tray, true);
        }
        else
        {
            Config::setConfig(Config::hide_to_tray, false);
        }
        Config::updateConfig(Config::hide_to_tray);
    });

    hide_layout->addWidget(hide_button);
    QHBoxLayout* close_layout = new QHBoxLayout();
    close_layout->addWidget(close_button);
    grid_layout->addWidget(warning_icon, 0, 0);
    grid_layout->addLayout(hide_layout, 0, 1);
    grid_layout->addLayout(close_layout, 1, 1);
    layout->addLayout(grid_layout);
    QHBoxLayout* button_layout = new QHBoxLayout();
    QDialogButtonBox* buttonBox = new QDialogButtonBox(this);//右下角按钮设置
    QPushButton* ok = new QPushButton(this);
    ok->setText("确认");
    ok->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);
    QPushButton* close = new QPushButton(this);
    close->setText("关闭");
    connect(ok, &QPushButton::clicked, this, [=](){
        if(Config::getConfig<bool>(Config::hide_to_tray))
        {
//            area->save_temp();
            emit hide_paint();
            is_close = false;
            this->close();
        }
//        else if(!area->pic_save)
//        {
//            int ans = QMessageBox::warning(
//                        this,
//                        "简截",
//                        "是否保存该截图\n可以保存为png、jpg、jpeg文件",
//                        QMessageBox::Cancel | QMessageBox::No| QMessageBox::Ok);
//            if(ans == QMessageBox::Ok)
//            {
//                QString file_name = QFileDialog::getSaveFileName(this,
//                                                                 "保存",
//                                                                 History::instance()->get_last_directory(),
//                                                                 "图片(*.bmp *.jpg *.jpeg *.png);;所有文件(*)");
//                area->save(file_name);
//                hide();
//                WindowManager::close();
//            }
//            else if(ans == QMessageBox::No)
//            {
//                hide();
//                WindowManager::close();
//            }
//            else
//            {
//                this->close();
//            }
//        }
        else
        {
            WindowManager::close();
        }
    });
    connect(close, &QPushButton::clicked, this, [=](){
        this->close();
    });
    buttonBox->addButton(ok, QDialogButtonBox::AcceptRole);
    buttonBox->addButton(close, QDialogButtonBox::RejectRole);
    QLabel* next_show_label = new QLabel("不再提示此消息", this);
    QRadioButton* next_show_button = new QRadioButton(this);
    next_show_button->setCheckable(true);
    next_show_button->setAutoExclusive(false);
    next_show_button->setChecked(!Config::getConfig<bool>(Config::show_close_dialog));
    connect(next_show_button, &QRadioButton::toggled, this, [=](bool clicked){
        Config::setConfig(Config::show_close_dialog, !clicked);
        Config::updateConfig(Config::show_close_dialog);
    });
    button_layout->addWidget(next_show_label);
    button_layout->addWidget(next_show_button);
    button_layout->addWidget(buttonBox, 0, Qt::AlignRight);

    layout->addLayout(grid_layout);
    layout->addLayout(button_layout);
    setLayout(layout);
}

void Close_dialog::closeEvent(QCloseEvent* event)
{
    if(is_close)
        WindowManager::close();
}
