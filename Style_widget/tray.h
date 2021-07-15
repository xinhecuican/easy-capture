#ifndef TRAY_H
#define TRAY_H
#include<QSystemTrayIcon>

class Tray : public QSystemTrayIcon
{
public:
    Tray();
    Tray(QObject* parent);
};

#endif // TRAY_H
