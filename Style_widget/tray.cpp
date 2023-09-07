#include "tray.h"
#include "Manager/WindowManager.h"
#include<QMenu>
#include "Helper/common.h"

Tray::Tray() {

}

Tray::Tray(QObject* parent) : QSystemTrayIcon(parent) {
    setIcon(QIcon(":/image/avator.png"));
    setToolTip("简截");
    connect(this, &QSystemTrayIcon::activated, this, [=](QSystemTrayIcon::ActivationReason reason) {
        switch(reason) {
        case DoubleClick:
            changeWindowHelper();
            break;
        case Context:
//            contextMenu()->show();
            break;
        default:
            break;
        }
    });
}
