#ifndef REPLY_TIMEOUT_H
#define REPLY_TIMEOUT_H
#include "QObject"
#include "QTimer"
#include "QNetworkReply"
class Reply_timeout : public QObject {


 Q_OBJECT


public:

  Reply_timeout(QNetworkReply *reply, const int timeout) : QObject(reply)
  {
      Q_ASSERT(reply);
      if (reply && reply->isRunning())
      {
          QTimer::singleShot(timeout, this, SLOT(onTimeout()));
      }
  }
signals:
  void timeout();  // 超时信号 - 供进一步处理
  void deadline();
private slots:
  void onTimeout()
  {
      // 处理超时
      QNetworkReply *reply = static_cast<QNetworkReply*>(parent());
      if (reply->isRunning())
      {
          reply->abort();
          reply->deleteLater();
          emit timeout();
      }
      emit deadline();
  }
};
#endif // REPLY_TIMEOUT_H
