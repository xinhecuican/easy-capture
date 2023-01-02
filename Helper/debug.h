#ifndef DEBUG_H
#define DEBUG_H
#include<QString>
#include<QImage>

class Debug
{
public:
    static void show_error_message(QString message);
    static void debug_print_warning(QString message);
    static void showImage(QImage image);
    static void beginTimer(QString name);
    static void endTimer();
    static qint64 getTime();
private:
    struct TimerElement
    {
        QString name;
        qint64 beginTime;
    };
    static QMap<int, QList<TimerElement>> timerName;
};

#endif // DEBUG_H
