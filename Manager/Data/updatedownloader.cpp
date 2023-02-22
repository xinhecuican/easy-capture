#include "updatedownloader.h"
#include <QDebug>
#include "Helper/debug.h"
#include <QDir>
#include "Manager/config.h"
#include "JlCompress.h"


UpdateDownloader::UpdateDownloader(QList<Update_data> data, QObject* parent) : QObject(parent)
{
    this->data = data;
    updateSum = data.size() - 1;
    timeoutTimer = new QTimer(this);
    currentReceive = 0;
    timerReceive = 0;
    connect(timeoutTimer, &QTimer::timeout, this, [=](){
        if(timerReceive == currentReceive){
            reply->abort();
        }
        else{
            qDebug() << currentReceive;
            timerReceive = currentReceive;
        }
    });
    qDebug() << "开始更新";
}

void UpdateDownloader::start(){
    request.setUrl(this->data[updateSum].get_url());
    qInfo() << this->data[updateSum].get_url();

//    request.setAttribute(QNetworkRequest::FollowRedirectsAttribute, true);
//    request.setRawHeader("Accept","text/html,application/xhtml+xml,application/xml;q=0.9,image/avif,image/webp,image/apng,*/*;q=0.8,application/signed-exchange;v=b3;q=0.9");
//    request.setRawHeader("User-Agent","Mozilla/5.0 (Windows NT 10.0) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/86.0.4240.198 Safari/537.36");
    reply = manager.get(request);
    timeoutTimer->start(30000);
    connect(reply, &QNetworkReply::downloadProgress, this, [=](qint64 bytesReceived, qint64 bytesTotal){
        currentReceive = bytesReceived;
    });
    connect(reply, &QNetworkReply::finished, this, [=](){
        if (reply->error())
        {
            qWarning() << "更新文件获取失败, 错误码: " + reply->errorString();
            reply->deleteLater();
            emit failure();
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
            QStringList slist = this->data[updateSum].get_url().split('/');
            QFileInfo file_info(slist.last());
            int sum;
            this->data[updateSum].decompress_version(&sum);
            QString file_name = "Data/Update/" + QString::number(sum) + "." + file_info.suffix();
            fileNames.append(QString::number(sum) + "." + file_info.suffix());
            QFile file(file_name);
            if(file.exists(file_name))
            {
                file.remove();
            }
            file.setFileName(file_name);
            if(!file.open(QIODevice::ReadWrite | QIODevice::Truncate))
            {
                qWarning("文件打开失败\n位置Update_dialog");
                Config::setConfig(Config::need_update, false);
                emit failure();
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
            updateSum--;
            if(updateSum < 0)
            {
                Config::setConfig(Config::need_update, true);
                Config::update_config(Config::need_update);
                emit success();
                QFile file("Data/Update/update_list.txt");
                if(!file.open(QIODevice::ReadWrite | QIODevice::Truncate))
                {
                    return;
                }
                QTextStream stream(&file);
                for(int i=fileNames.size()-1; i>=0; i--)
                {
                    stream << fileNames[i] << "\n";
                }
                file.close();
            }
            else
            {
                start();
            }
        }
        else if(statusCode >=300 && statusCode < 400)
        {
            qWarning("文件需要重定向\n位置:Update_dialog");
            emit failure();
        }
    });
}
