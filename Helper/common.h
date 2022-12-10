#ifndef HELPER_COMMON_H
#define HELPER_COMMON_H
#include<QPoint>
#include<QString>
#include<QFile>

struct MRect
{
    QPoint topleft;
    QPoint topright;
    QPoint bottomleft;
    QPoint bottomright;
    MRect(){};
};

static QString getQSS(QString path)
{
    QFile file(path);
    file.open(QFile::ReadOnly);
    QString str = file.readAll();
    file.close();
    return str;
}

#endif // COMMON_H
