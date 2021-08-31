#include "update.h"
#include<QJsonObject>
#include<QDateTime>
#include<QNetworkAccessManager>
#include "Helper/Serialize.h"
#include "Data/update_dialog.h"
#include "Helper/debug.h"
#include "config.h"
#include "Data/Reply_timeout.h"
#include<QTextCodec>
#include<QMessageBox>
#include "key_manager.h"
#include "window_manager.h"

Update::Update()
{
    newest_data = Update_data();
    manager = new QNetworkAccessManager(this);
}

Update::~Update()
{
    _instance = NULL;
}

Update* Update::_instance = NULL;
Update_data Update::now_version = Update_data("0.2.0",
"https://cdn.jsdelivr.net/gh/xinhecuican/Resources/easy_capture_version/0.2.0.zip",
                                              "",
                                              "1. 实现了滚动截图\n"
                                              "2. 将矩形窗口和活动窗口截图合并\n"
                                              "3. 修复了绘图窗口复制到剪切板导致的内存泄露\n"
                                              "4. 修复了无法自动启动的错误\n"
                                              "5. 体验优化及bug修复\n"
                                              "注意：由于替换为64位，所有的库文件都需要重新下载，而更新最多支持20mb文件，可能无法下载\n"
                                              "因此推荐去https://xinhecuican.github.io/post/f0fbe9f2.html下载");

void Update::serialized(QJsonObject *json)//append增添版本时用
{
    QJsonObject child;
    now_version.set_time(QDateTime::currentDateTime().toString());
    now_version.serialized(&child);
    (*json)[QString::number((*json)["update_sum"].toInt())] = child;
    (*json)["update_sum"] = (*json)["update_sum"].toInt() + 1;//更新数量增加
}

void Update::deserialized(QJsonObject *json)
{
    QJsonObject newest_version =
            (*json)[QString::number((*json)["update_sum"].toInt()-1)].toObject();
    newest_data.deserialized(&newest_version);
}

void Update::check_update()
{
    Config::set_config(Config::last_update_time, QDateTime::currentSecsSinceEpoch() / 60);
    start_request(QUrl("https://cdn.jsdelivr.net/gh/xinhecuican/Resources/easy_capture_version/update.json"));
}

void Update::start_request(const QUrl &url)
{
    request.setUrl(url);
    request.setAttribute(QNetworkRequest::FollowRedirectsAttribute, true);
    request.setRawHeader("Accept","text/html,application/xhtml+xml,application/xml;q=0.9,image/avif,image/webp,image/apng,*/*;q=0.8,application/signed-exchange;v=b3;q=0.9");
    request.setRawHeader("User-Agent","Mozilla/5.0 (Windows NT 10.0) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/86.0.4240.198 Safari/537.36");
    reply = manager->get(request);
    Reply_timeout* timeout = new Reply_timeout(reply, 10000);
    connect(timeout, &Reply_timeout::timeout, this, [=](){
        qDebug() << "timeout";
    });
    connect(reply, &QNetworkReply::finished, this, [=](){
        if (reply->error())
        {
            Debug::show_error_message("更新检测失败");
            reply->deleteLater();
            return;
        }
        int statusCode  = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
        // <3>判断是否需要重定向
        if (statusCode >= 200 && statusCode <300)
        {
            QTextCodec *codec = QTextCodec::codecForName("utf8");
            QDir dir("Data/Temp");
            if(!dir.exists())
            {
                dir.mkpath(dir.absolutePath());
            }
            QFile file("Data/Temp/update_msg.json");
            if (!file.open(QIODevice::ReadWrite | QIODevice::Text | QIODevice::Truncate))
            {
                qDebug() << "file open error!";
                return ;
            }
            QTextStream out(&file);
            out.setCodec("UTF-8");
            out<< codec->toUnicode(reply->readAll());
            file.close();
            Serialize::deserialize("Data/Temp/update_msg.json", this);
            if(newest_data > now_version)
            {
                ///TODO: 更新提示面板
                Update_dialog* dialog = new Update_dialog(newest_data, this);
                dialog->show();
            }
        }
        else if(statusCode >= 300 && statusCode < 400)
        {
            const QVariant redirectionTarget = reply->attribute(QNetworkRequest::RedirectionTargetAttribute);
            // 检测是否需要重定向，如果不需要则读数据
            if (!redirectionTarget.isNull()) 
            {
                const QUrl redirectedUrl = url.resolved(redirectionTarget.toUrl());
                reply->deleteLater();
                reply = nullptr;
                start_request(redirectedUrl);
                return;
            }
        }
    });
}

void Update::update()
{
    QTimer::singleShot(1000, this, SLOT(on_update()));
}

void Update::on_update()
{
    QFile file("Data/update_res.txt");
    if(!file.open(QIODevice::ReadOnly))
    {
        return;
    }
    int ans = file.readAll().toInt();
    if(file.exists() && ans)
    {
        Config::update_all();
        Key_manager::update_all();
        Config::set_config(Config::need_update, 0);
        Config::update_config(Config::need_update);
        file.remove();
    }
    if(Config::get_config(Config::need_update) == 1)
    {
        int ans = QMessageBox::question(this, "更新提示", "是否进行更新");
        if(ans == QMessageBox::Yes)
        {
            if(QProcess::startDetached("Upgrate.exe"))//开启更新程序
                Window_manager::close();
            else
                Debug::debug_print_warning("更新程序未启动");
        }
    }
}

void Update::save()
{
    Serialize::append("Data/update.json", this);
}

void Update::load()
{

}
