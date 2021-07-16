#include "update_dialog.h"
#include<QHBoxLayout>
#include<QLabel>
#include<QDialogButtonBox>
#include<QPushButton>
#include <QtNetwork>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include "Helper/debug.h"
#include "Manager/config.h"
#include<QFile>
#include<QDir>

Update_dialog::Update_dialog()
{

}

Update_dialog::Update_dialog(Update_data data, QWidget* parent) : QDialog(parent)
{
    this->data = data;
    setAttribute(Qt::WA_DeleteOnClose, true);
    QVBoxLayout* layout = new QVBoxLayout(this);
    setLayout(layout);
    QLabel* label = new QLabel("有一个更新可用，版本号" + data.get_version(), this);
    label->setAlignment(Qt::AlignHCenter);
    label->setFont(QFont("Microsoft YaHei", 15, 4, true));
    QLabel* label1 = new QLabel("新功能", this);
    QLabel* label2 = new QLabel(data.get_description(), this);
    QVBoxLayout* main_area_layout = new QVBoxLayout(this);
    main_area_layout->addWidget(label);
    main_area_layout->addSpacing(10);
    main_area_layout->addWidget(label1);
    main_area_layout->addWidget(label2);
    QDialogButtonBox* buttonBox = new QDialogButtonBox(this);//右下角按钮设置
    QPushButton* ok = new QPushButton(this);
    ok->setText("确认");
    ok->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);
    connect(ok, &QPushButton::clicked, this, [=](){
        QNetworkRequest request;
        request.setUrl(this->data.get_url());
        request.setAttribute(QNetworkRequest::FollowRedirectsAttribute, true);
        request.setRawHeader("Accept","text/html,application/xhtml+xml,application/xml;q=0.9,image/avif,image/webp,image/apng,*/*;q=0.8,application/signed-exchange;v=b3;q=0.9");
        request.setRawHeader("User-Agent","Mozilla/5.0 (Windows NT 10.0) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/86.0.4240.198 Safari/537.36");
        QNetworkAccessManager manager;
        QNetworkReply* reply = manager.get(request);
        connect(reply, &QNetworkReply::finished, this, [=](){
            if (reply->error())
            {
                Debug::show_error_message("更新文件获取失败");
                reply->deleteLater();
                return;
            }
            int statusCode  = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
            if (statusCode >= 200 && statusCode <300)
            {
                Config::set_config(Config::need_update, true);
                Config::update_config(Config::need_update);
                QDir dir;
                if(!dir.exists("Data/Update/"))
                {
                    dir.mkpath("Data/Update/");
                }
                QFile file("Data/Update/" + this->data.get_version() + ".zip");
                if(!file.open(QIODevice::ReadWrite | QIODevice::Truncate))
                {
                    Debug::debug_print_warning("文件打开失败\n位置Update_dialog");
                }
                file.write(reply->readAll());
                file.close();
            }
            else if(statusCode >=300 && statusCode < 400)
            {
                Debug::debug_print_warning("文件需要重定向\n位置:Update_dialog");
            }
        });
    });
    QPushButton* close = new QPushButton(this);
    close->setText("关闭");
    connect(close, &QPushButton::clicked, this, [=](){
        this->close();
    });
    buttonBox->addButton(ok, QDialogButtonBox::AcceptRole);
    buttonBox->addButton(close, QDialogButtonBox::RejectRole);
    QHBoxLayout* button_layout = new QHBoxLayout(this);
    button_layout->addWidget(buttonBox);
    button_layout->setDirection(QBoxLayout::RightToLeft);
    layout->addLayout(main_area_layout);
    layout->addLayout(button_layout);
}


