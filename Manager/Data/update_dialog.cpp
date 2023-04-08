#include "update_dialog.h"
#include<QHBoxLayout>
#include<QLabel>
#include<QDialogButtonBox>
#include<QPushButton>
#include "Helper/debug.h"
#include "Manager/config.h"
#include<QFile>
#include<QDir>
#include "Reply_timeout.h"
#include "JlCompress.h"
#include<QStringList>

Update_dialog::Update_dialog()
{

}

Update_dialog::~Update_dialog()
{
}

Update_dialog::Update_dialog(QList<UpdateData> data, QWidget* parent) : QDialog(parent)
{
    downloader = new UpdateDownloader(data, this);
    setAttribute(Qt::WA_DeleteOnClose, true);
    QVBoxLayout* layout = new QVBoxLayout(this);
    setLayout(layout);
    QLabel* label = new QLabel("有一个更新可用，版本号" + data[0].get_version(), this);
    label->setAlignment(Qt::AlignHCenter);
    label->setFont(QFont("Microsoft YaHei", 15, 4, true));
    QLabel* label1 = new QLabel("新功能:", this);
    QLabel* label2 = new QLabel(data[0].get_description(), this);
    label2->setTextInteractionFlags(Qt::TextSelectableByMouse);
    QLabel* label3 = new QLabel("更新时间: " + QDateTime::fromString(data[0].get_time()).toString("yyyy.MM.dd"), this);
    QVBoxLayout* main_area_layout = new QVBoxLayout();
    main_area_layout->setAlignment(Qt::AlignTop);
    main_area_layout->setContentsMargins(20, 0, 20, 10);
    main_area_layout->addWidget(label);
    main_area_layout->addSpacing(10);
    main_area_layout->addWidget(label3);
    main_area_layout->addSpacing(20);
    main_area_layout->addWidget(label1);
    main_area_layout->addSpacing(10);
    main_area_layout->addWidget(label2);
    QDialogButtonBox* buttonBox = new QDialogButtonBox(this);//右下角按钮设置
    QPushButton* ok = new QPushButton(this);
    ok->setText("更新");
    ok->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);
    connect(ok, &QPushButton::clicked, this, [=](){
        downloader->start();
        this->hide();
    });
    QPushButton* close = new QPushButton(this);
    close->setText("关闭");
    connect(close, &QPushButton::clicked, this, [=](){
        this->close();
    });
    connect(downloader, &UpdateDownloader::success, this, [=](){
        emit download_finished();
    });
    buttonBox->addButton(ok, QDialogButtonBox::AcceptRole);
    buttonBox->addButton(close, QDialogButtonBox::RejectRole);
    QHBoxLayout* button_layout = new QHBoxLayout();
    button_layout->addWidget(buttonBox);
    button_layout->setDirection(QBoxLayout::RightToLeft);
    layout->addLayout(main_area_layout);
    layout->addLayout(button_layout);
}
