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
    delete timeout;
}

Update_dialog::Update_dialog(QList<Update_data> data, QWidget* parent) : QDialog(parent)
{
    this->data = data;
    update_sum = data.size()-1;
    timeout = new Reply_timeout(10000);
    connect(timeout, &Reply_timeout::deadline, this, [=](){
        Debug::show_error_message("更新超时，请检查网络设置");
        this->close();
    });
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
        get_one_update();
        this->hide();
    });
    QPushButton* close = new QPushButton(this);
    close->setText("关闭");
    connect(close, &QPushButton::clicked, this, [=](){
        this->close();
    });
    buttonBox->addButton(ok, QDialogButtonBox::AcceptRole);
    buttonBox->addButton(close, QDialogButtonBox::RejectRole);
    QHBoxLayout* button_layout = new QHBoxLayout();
    button_layout->addWidget(buttonBox);
    button_layout->setDirection(QBoxLayout::RightToLeft);
    layout->addLayout(main_area_layout);
    layout->addLayout(button_layout);
}

void Update_dialog::get_one_update()
{
    request.setUrl(this->data[update_sum].get_url());
    qDebug() << this->data[update_sum].get_url();

//    request.setAttribute(QNetworkRequest::FollowRedirectsAttribute, true);
//    request.setRawHeader("Accept","text/html,application/xhtml+xml,application/xml;q=0.9,image/avif,image/webp,image/apng,*/*;q=0.8,application/signed-exchange;v=b3;q=0.9");
//    request.setRawHeader("User-Agent","Mozilla/5.0 (Windows NT 10.0) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/86.0.4240.198 Safari/537.36");
    reply = manager.get(request);
    timeout->reset(reply, 500000);
    connect(reply, &QNetworkReply::finished, this, [=](){
        if (reply->error())
        {
            Debug::show_error_message("更新文件获取失败, 错误码: " + reply->errorString());
            reply->deleteLater();
            return;
        }
        int statusCode  = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
        if (statusCode >= 200 && statusCode <300)
        {
            QDir dir("Data/Update/");
            if(!dir.exists("Data/Update/"))
            {
                dir.mkpath(dir.absolutePath());
            }
            QStringList slist = this->data[update_sum].get_url().split('/');
            QFileInfo file_info(slist.last());
            int sum;
            this->data[update_sum].decompress_version(&sum);
            QString file_name = "Data/Update/" + QString::number(sum) + "." + file_info.suffix();
            file_names.append(QString::number(sum) + "." + file_info.suffix());
            QFile file(file_name);
            if(file.exists(file_name))
            {
                file.remove();
            }
            file.setFileName(file_name);
            if(!file.open(QIODevice::ReadWrite | QIODevice::Truncate))
            {
                Debug::debug_print_warning("文件打开失败\n位置Update_dialog");
                Config::set_config(Config::need_update, false);
                this->close();
                return;
            }
            file.write(reply->readAll());
            file.close();
            QStringList list = JlCompress::getFileList(file_name);
            if(list.contains("Upgrate.exe"))
            {
                JlCompress::extractFile(file_name,
                                        "Upgrate.exe", "Upgrate.exe");
            }
            update_sum--;
            if(update_sum < 0)
            {
                Config::set_config(Config::need_update, true);
                Config::update_config(Config::need_update);
                emit download_finished();
                QFile file("Data/Update/update_list.txt");
                if(!file.open(QIODevice::ReadWrite | QIODevice::Truncate))
                {
                    this->close();
                    return;
                }
                QTextStream stream(&file);
                for(int i=file_names.size()-1; i>=0; i--)
                {
                    stream << file_names[i] << "\n";
                }
                file.close();
                this->close();
            }
            else
            {
                get_one_update();
            }
        }
        else if(statusCode >=300 && statusCode < 400)
        {
            Debug::debug_print_warning("文件需要重定向\n位置:Update_dialog");
            this->close();
        }
    });
}
