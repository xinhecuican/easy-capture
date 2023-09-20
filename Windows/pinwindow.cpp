#include "pinwindow.h"
#include "Helper/imagehelper.h"
#include "Manager/WindowManager.h"

PinWindow::PinWindow(QWidget* parent) : WindowBase(parent)
{
    setWindowFlags(Qt::SubWindow);
    setWindowFlag(Qt::FramelessWindowHint);
    setAttribute(Qt::WA_TranslucentBackground);
    setAttribute(Qt::WA_DeleteOnClose);
    setAttribute(Qt::WA_OpaquePaintEvent );
    setWindowFlags(windowFlags() | Qt::WindowStaysOnTopHint);
    setGeometry(ImageHelper::getCurrentScreen()->geometry());
}

void PinWindow::onWindowCancel(){
    show();
}

void PinWindow::onWindowSelect(){

}

void PinWindow::receiveData(QVariant data1, QVariant data2){
    QImage image = data1.value<QImage>();
    QRect bound = data2.value<QRect>();
    PinWidget* pinWidget = new PinWidget(image, bound, this);
    connect(pinWidget, &PinWidget::closeNow, this, [=](){
        pinWidgets.removeOne(pinWidget);
        pinWidget->deleteLater();
        if(pinWidgets.size() == 0){
            if(WindowManager::getNowWindow() == "PinWindow"){
                WindowManager::changeWindow("tray");
            }
            WindowManager::closeWindow("PinWindow");
        }
    });
    connect(pinWidget, &PinWidget::closeAll, this, [=](){
        for(PinWidget* widget : pinWidgets){
            widget->deleteLater();
        }
        pinWidgets.clear();
        if(WindowManager::getNowWindow() == "PinWindow"){
            WindowManager::changeWindow("tray");
        }
        WindowManager::closeWindow("PinWindow");
    });
    pinWidget->show();
    pinWidgets.append(pinWidget);
}

bool PinWindow::controlWindowClose(){
    return true;
}
