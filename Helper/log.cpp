
#include "log.h"
#include <QTextCodec>

QFile *gFileLog = NULL;

QString msgHead[] = {
    "Debug   ",
    "Warning ",
    "Critical",
    "Fatal   ",
    "Info    "
};

void log(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    QString current_date_time = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss:zzz ddd");
    if(gFileLog){
        QTextStream tWrite(gFileLog);
        QString msgText="%1 | %6 | %2:%3, %4 | %5\n";
        msgText = msgText.arg(msgHead[type]).arg(context.file).arg(context.line).arg(context.function).arg(msg.toStdString().c_str()).arg(current_date_time);
//        gFileLog->write(msgText.toLocal8Bit(), msgText.length());
        tWrite << msgText;
    }else{
//        fprintf(stderr, "%s | %s | %s:%u, %s | %s\n", msgHead[type], current_date_time.toLocal8Bit().constData(), context.file, context.line, context.function, localMsg.constData());
    }

}

void logSysInit(QString filePath)
{
    gFileLog = new QFile(filePath);
    if (!gFileLog->open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate)){
        return;
    }
    qInstallMessageHandler(log);
}

void logSysInit()
{
    qInstallMessageHandler(log);
}
