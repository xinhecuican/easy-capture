#ifndef REPLY_TIMEOUT_H
#define REPLY_TIMEOUT_H
#include "QObject"
#include "QTimer"
#include "QNetworkReply"
#include<QPointer>
class Reply_timeout : public QObject {


 Q_OBJECT


public:

  Reply_timeout(const int time, QObject* parent): QObject(parent)
  {
      reply = NULL;
      connect(&timer, &QTimer::timeout, this, [=](){
          // 处理超时
          if (reply != NULL && reply->isRunning())
          {
              reply->deleteLater();
              reply = NULL;
              emit timeout();
          }
          emit deadline();
          timer.stop();
      });
      if (reply && reply->isRunning())
      {
          timer.start(time);
      }
  }

  void reset(QNetworkReply *reply, int timeout)
  {
      this->reply = reply;
      connect(reply, &QNetworkReply::finished, this, [=](){
          if(timer.isActive())
          {
              timer.stop();
          }
          this->reply = NULL;
      });
      connect(reply, static_cast<void (QNetworkReply::*)(QNetworkReply::NetworkError)>(&QNetworkReply::error),
              this, [=](QNetworkReply::NetworkError code){
          qDebug() << code;
          if(timer.isActive())
          {
              timer.stop();
          }
          this->reply = NULL;
      });
      if(timer.isActive())
      {
          timer.stop();
      }
      timer.start(timeout);
  }
signals:
  void timeout();  // 超时信号 - 供进一步处理
  void deadline();
private slots:
  void onTimeout()
  {

  }
private:
  QTimer timer;
  QPointer<QNetworkReply> reply;
};

#endif // REPLY_TIMEOUT_H
