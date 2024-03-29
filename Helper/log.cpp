
#include "log.h"
#include <QTextCodec>
#include "debug.h"

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
        if(type == 1 || type == 2 || type == 3){
            tWrite << Debug::backtrace();
        }
    }else{
//        fprintf(stderr, "%s | %s | %s:%u, %s | %s\n", msgHead[type], current_date_time.toLocal8Bit().constData(), context.file, context.line, context.function, localMsg.constData());
    }

}

void logSysInit(QString filePath)
{
    createDirWithFile(filePath);
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
