#ifndef REPLY_TIMEOUT_H
#define REPLY_TIMEOUT_H
#include "QObject"
#include "QTimer"
#include "QNetworkReply"
class Reply_timeout : public QObject {


 Q_OBJECT


public:

  Reply_timeout(QNetworkReply *reply, const int time) : QObject(reply)
  {
      Q_ASSERT(reply);
      connect(&timer, &QTimer::timeout, this, [=](){
          // 处理超时
          QNetworkReply *reply = static_cast<QNetworkReply*>(parent());
          if (reply->isRunning())
          {
              reply->abort();
              reply->deleteLater();
              emit timeout();
          }
          emit deadline();
      });
      if (reply && reply->isRunning())
      {
          timer.start(time);
      }
  }

  void reset(int timeout)
  {
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
};

#endif // REPLY_TIMEOUT_H
