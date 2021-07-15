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
            Window_manager::show_now();
            break;
        default:
            break;
        }
    });

    QMenu* menu = new QMenu();
    QAction* close = new QAction(this);
    close->setText("关闭");
    connect(close, &QAction::triggered, this, [=](){
        Window_manager::close();
    });
    menu->addAction(close);
    setContextMenu(menu);
}
