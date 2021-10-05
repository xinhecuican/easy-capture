#ifndef REPLY_TIMEOUT_H
#define REPLY_TIMEOUT_H
#include "QObject"
#include "QTimer"
#include "QNetworkReply"
#include<QPointer>
class Reply_timeout : public QObject {


 Q_OBJECT


public:

  Reply_timeout(const int time)
  {
      reply = NULL;
      connect(&timer, &QTimer::timeout, this, [=](){
          // 处理超时
          if (reply != NULL && reply->isRunning())
          {
              reply->abort();
              reply->deleteLater();
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
