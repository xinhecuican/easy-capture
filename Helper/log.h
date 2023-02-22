
#ifndef LOG_H
#define LOG_H

#include <QFile>
#include <QMessageLogger>
#include <qlogging.h>
#include <QFile>
#include <QTextStream>
#include <QDateTime>

void logSysInit(QString filePath);
void logSysInit();


#endif // LOG_H
