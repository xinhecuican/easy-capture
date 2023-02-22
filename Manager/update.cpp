#include "update.h"
#include<QJsonObject>
#include<QDateTime>
#include<QNetworkAccessManager>
#include "Helper/Serialize.h"
#include "Helper/debug.h"
#include "config.h"
#include "Data/Reply_timeout.h"
#include<QTextCodec>
#include<QMessageBox>
#include "key_manager.h"
#include "window_manager.h"
#include "Paint/Widgets/history.h"

Update::Update()
{
    newest_data = Update_data();
    manager = new QNetworkAccessManager(this);
    reconnect_times = 0;
    timer = new QTimer(this);
    currentReceive = 0;
    timerReceive = 0;
    updateState = IDLE;
    timeoutTimer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, [=](){
        reconnect_times++;
        if(reconnect_times > 5)
        {
            timer->stop();
            onFinish();
            return;
        }
        check_update();
    });
    connect(timeoutTimer, &QTimer::timeout, this, [=](){
        if(timerReceive == currentReceive){
            reply->abort();
            updateState = IDLE;
            updateStateChange(updateState);
        }
        else{
            timerReceive = currentReceive;
        }
    });
}

Update::~Update()
{
    if(_instance != NULL)
    {
        _instance = NULL;
    }
}

Update* Update::_instance = NULL;
Update_data Update::now_version = Update_data("0.4.3",
"http://121.37.81.150:8200/easycapture/update/0.4.4.zip", "",
                                              "1. 添加ocr功能");

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
    int newest_num = (*json)["update_sum"].toInt();
    QJsonObject newest_version =
            (*json)[QString::number(newest_num-1)].toObject();
    newest_data.deserialized(&newest_version);
    for(int i=newest_num-1; i>=0; i--)//将所有比当前版本新的版本都记录
    {
        Update_data data;
        QJsonObject version = (*json)[QString::number(i)].toObject();
        data.deserialized(&version);
        if(data == now_version)
        {
            break;
        }
        qDebug() << data.get_version();
        data_list.append(data);
    }
}

void Update::check_update()
{
    updateState = CHECKING;
    emit updateStateChange(updateState);
    Config::setConfig(Config::last_update_time, QDateTime::currentSecsSinceEpoch() / 60);
    start_request(QUrl("http://121.37.81.150:8200/easycapture/update/update.json?download=true"));
}

void Update::start_request(const QUrl &url)
{
    request.setUrl(url);
    request.setAttribute(QNetworkRequest::FollowRedirectsAttribute, true);
    request.setRawHeader("Accept","text/html,application/xhtml+xml,application/xml;q=0.9,image/avif,image/webp,image/apng,*/*;q=0.8,application/signed-exchange;v=b3;q=0.9");
    request.setRawHeader("User-Agent","Mozilla/5.0 (Windows NT 10.0) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/86.0.4240.198 Safari/537.36");
    manager->clearAccessCache();
    reply = manager->get(request);
    timeoutTimer->start(30000);
    connect(reply, &QNetworkReply::downloadProgress, this, [=](qint64 bytesReceived, qint64 bytesTotal){
        currentReceive = bytesReceived;
    });
    connect(reply, &QNetworkReply::finished, this, [=](){
        if (reply->error())
        {
            qDebug() << "error";
            timer->start(2000);
            reply->deleteLater();
            updateState = IDLE;
            updateStateChange(updateState);
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
                if(Config::getConfig<bool>(Config::show_update_box)){
                    dialog = new Update_dialog(data_list, this);
                    connect(dialog, &Update_dialog::download_finished, this, [=](){
                        on_update();
                    });
                    dialog->show();
                }
                else{
                    UpdateDownloader* downloader = new UpdateDownloader(data_list, this);
                    downloader->start();
                    connect(downloader, &UpdateDownloader::success, this, [=](){
                        on_update();
                    });
                    connect(downloader, &UpdateDownloader::failure, this, [=](){
                        updateState = IDLE;
                        emit updateStateChange(updateState);
                    });
                }
                updateState = UPDATING;
                updateStateChange(updateState);
            }
            if(timer->isActive())
            {
                timer->stop();
            }
            if(timeoutTimer->isActive()){
                timeoutTimer->stop();
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
    if(file.open(QIODevice::ReadOnly))
    {
        int ans = file.readAll().toInt();
        if(file.exists() && ans)
        {
            Config::update_all();
            Key_manager::update_all();
            History::instance()->update();
            Config::setConfig(Config::need_update, 0);
            Config::update_config(Config::need_update);
            file.remove();
        }
    }

    if(Config::getConfig<bool>(Config::need_update) == 1)
    {
        if(Config::getConfig<bool>(Config::show_update_box)){
            int ans = QMessageBox::question(this, "更新提示", "是否进行更新");
            if(ans == QMessageBox::Yes)
            {
                qInfo() << "正在更新";
                if(QProcess::startDetached("Upgrate.exe"))//开启更新程序
                    Window_manager::close();
                else
                    qWarning("更新程序未启动");
                onFinish();
            }
        }
        else{
            qInfo() << "正在更新";
            QStringList args;
            args << "terminal";
            if(QProcess::startDetached("Upgrate.exe", args))//开启更新程序
                Window_manager::close();
            else
                qWarning("更新程序未启动");
            onFinish();
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

void Update::onFinish()
{
    if(_instance != NULL)
    {
        _instance->deleteLater();
    }
}
