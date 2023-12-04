#ifndef HELPER_COMMON_H
#define HELPER_COMMON_H
#include<QPoint>
#include<QString>
#include<QFile>
#include <QDir>
#include <QDebug>
#include "../Manager/WindowManager.h"
#include "../Manager/config.h"
#include <QGuiApplication>
#include <QScreen>

struct MRect {
    QPoint topleft;
    QPoint topright;
    QPoint bottomleft;
    QPoint bottomright;
    MRect() {};
};

static QString getQSS(QString path) {
    QFile file(path);
    if(!file.open(QFile::ReadOnly)) {
        file.close();
        return "";
    }
    QString str = file.readAll();
    file.close();
    return str;
}

static void createDirWithFile(QString filePath) {
    int index = filePath.lastIndexOf('/');
    QDir dir(filePath.mid(0, index));
    dir.mkpath(dir.absolutePath());
}

static void changeWindowHelper(){
    if(Config::getConfig<int>(Config::capture_mode) == (int)Config::SCROLL_CAPTURE){
        WindowManager::changeWindow("ScrollerWindow");
    }
    else{
        WindowManager::changeWindow("CaptureWindow");
    }
}

static int pt2px(double pt){
    QPoint pos = QCursor::pos();
    if(pos.x() < 0 || pos.y() < 0){
        pos = QPoint(0, 0);
    }
    QScreen* screen = QGuiApplication::screenAt(pos);
    if(screen != NULL)
        return (int)(pt / 72 * screen->physicalDotsPerInch());
    else
        return pt;
}

static QPixmap loadSVG(QString name, int pt_w, int pt_h){

}
#endif // COMMON_H
