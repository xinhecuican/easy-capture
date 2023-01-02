#include "tray.h"
#include "Manager/window_manager.h"
#include<QMenu>

Tray::Tray()
{

}

Tray::Tray(QObject* parent) : QSystemTrayIcon(parent)
{
    setIcon(QIcon(":/image/avator.png"));
    setToolTip("简截");
    connect(this, &QSystemTrayIcon::activated, this, [=](QSystemTrayIcon::ActivationReason reason){
        switch(reason)
        {
        case DoubleClick:
            Window_manager::change_window("Capture_window");
            break;
        case Context:
            contextMenu()->show();
            break;
        default:
            break;
        }
    });
}
