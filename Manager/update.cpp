#include "update.h"
#include<QJsonObject>
#include<QDateTime>
#include<QNetworkAccessManager>
#include "Helper/Serialize.h"
#include "Data/update_dialog.h"
#include "Helper/debug.h"
#include "config.h"

Update::Update()
{
    newest_data = Update_data();
}

Update* Update::_instance = NULL;
Update_data Update::now_version = Update_data("0.0.1",
"https://raw.githubusercontent.com/xinhecuican/Resources/master/easy_capture_version/0.0.1.zip",
                                              "",
                                              "更新测试");

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
    newest_data.serialized(&newest_version);
}

void Update::check_update()
{
    Config::set_config(Config::last_update_time, QDateTime::currentSecsSinceEpoch() / 60);
    start_request(QUrl(""));
}

void Update::start_request(const QUrl &url)
{
    QNetworkAccessManager manager;
    request.setUrl(url);
    request.setAttribute(QNetworkRequest::FollowRedirectsAttribute, true);
    request.setRawHeader("Accept","text/html,application/xhtml+xml,application/xml;q=0.9,image/avif,image/webp,image/apng,*/*;q=0.8,application/signed-exchange;v=b3;q=0.9");
    request.setRawHeader("User-Agent","Mozilla/5.0 (Windows NT 10.0) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/86.0.4240.198 Safari/537.36");
    reply = manager.get(request);
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
            Serialize::deserialize_data(reply->readAll(), this);
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

void Update::save()
{
    Serialize::append("Data/update.json", this);
}

void Update::load()
{

}
