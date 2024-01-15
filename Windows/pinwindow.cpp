#include "pinwindow.h"
#include "Helper/imagehelper.h"
#include "Manager/WindowManager.h"
#include <QGuiApplication>

PinWindow::PinWindow(QWidget* parent) : WindowBase(parent)
{
    setWindowFlags(Qt::SubWindow);
    setWindowFlag(Qt::FramelessWindowHint);
    setAttribute(Qt::WA_TranslucentBackground);
    setAttribute(Qt::WA_DeleteOnClose);
    setAttribute(Qt::WA_OpaquePaintEvent );
    setWindowFlags(windowFlags() | Qt::WindowStaysOnTopHint);
    for(int i=0; i<QGuiApplication::screens().size(); i++){
        allGeometry = allGeometry.united(QGuiApplication::screens().at(i)->geometry());
    }
    setGeometry(allGeometry);
}

void PinWindow::onWindowCancel(){
    show();
}

void PinWindow::onWindowSelect(){

}

void PinWindow::receiveData(QVariant data1, QVariant data2){
    QImage image = data1.value<QImage>();
    QRect bound = data2.value<QRect>();
    bound.moveTo(bound.topLeft() + ImageHelper::getCurrentGeometry().topLeft() - allGeometry.topLeft());

    PinWidget* pinWidget = new PinWidget(image, bound, this);
    connect(pinWidget, &PinWidget::closeNow, this, [=](){
        pinWidgets.removeOne(pinWidget);
        pinWidget->deleteLater();
        if(pinWidgets.size() == 0){
            if(WindowManager::instance()->getNowWindow() == "PinWindow"){
                WindowManager::instance()->changeWindow("tray");
            }
            WindowManager::instance()->closeWindow("PinWindow");
        }
    });
    connect(pinWidget, &PinWidget::closeAll, this, [=](){
        for(PinWidget* widget : pinWidgets){
            widget->deleteLater();
        }
        pinWidgets.clear();
        if(WindowManager::instance()->getNowWindow() == "PinWindow"){
            WindowManager::instance()->changeWindow("tray");
        }
        WindowManager::instance()->closeWindow("PinWindow");
    });
    pinWidget->show();
    pinWidgets.append(pinWidget);
}

bool PinWindow::controlWindowClose(){
    return true;
}
