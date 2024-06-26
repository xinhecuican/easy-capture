#include "WindowManager.h"
#include <QDebug>
#include "../Helper/debug.h"
#include "config.h"
#include<QApplication>
#include<QThread>
#include "KeyManager.h"
#include "../MainFilter.h"

WindowManager::WindowManager()
    : activeWindow("tray"),
      previousWindow("")
{
    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &WindowManager::controlWindowClose);
}

//延时删除，加快窗口间的切换速度
void WindowManager::controlWindowClose() {
    int time = Config::getConfig<int>(Config::clear_interval);
    qint64 current_time = QDateTime::currentDateTime().currentSecsSinceEpoch();
    QList<WindowBase*> temp_list = QList<WindowBase*>();
    // 如果是设置则不销毁窗口，否则设置完成后绘图窗口可能会重置
    if(activeWindow != "tray" && windowList[activeWindow].window->disableWindowClose())return;
    qInfo() << activeWindow << endl;
    for(auto iter=windowList.begin(); iter!=windowList.end();) {
        if(current_time-iter.value().time >= time&&
            (iter.key() != activeWindow) && !iter.value().window->controlWindowClose()) {
            temp_list.append(iter.value().window);
            qInfo() << "delete " << iter.key();
            KeyManager::instance()->onWindowClose(iter.key());
            iter = windowList.erase(iter);
        } else {
            iter++;
        }
    }
    for(int i=0; i<temp_list.size(); i++) {
        temp_list[i]->onWindowClose();
        temp_list[i]->deleteLater();
    }
    if(windowList.size() == 0) {
        timer->stop();
    }
}

void WindowManager::closeWindow(QString name) {
    if(name == activeWindow) {
        qWarning("未关闭窗口调用closeWindow");
    } else if(windowList.find(name) != windowList.end()) {
        windowList[name].window->onWindowCancel();
        windowList[name].window->onWindowClose();
        windowList[name].window->deleteLater();
        windowList.remove(name);
        qInfo() << "关闭窗口" << name;
    }

}

void WindowManager::pushWindow(QString name, WindowBase *widget) {
    windowList[name] = createData(widget);
}

void WindowManager::changeWindow(QString name, QVariant data1, QVariant data2, bool closeCurrent) {
    qInfo() << name << " " << activeWindow;
    createWindow(name);
    if(activeWindow != name) {
        KeyManager::instance()->onWindowChangeBegin(activeWindow);
        if(name == "tray") {
            windowList[activeWindow].window->setWindowFlag(Qt::WindowSystemMenuHint, false);
        }
        if(activeWindow != "tray" && windowList.contains(activeWindow)) {
            windowList[activeWindow].time = QDateTime::currentDateTime().currentSecsSinceEpoch();
            windowList[activeWindow].window->hide();
            windowList[activeWindow].window->onWindowCancel();
            if (closeCurrent) {
                windowList[activeWindow].window->onWindowClose();
                windowList[activeWindow].window->deleteLater();
                windowList.remove(activeWindow);
                qInfo() << "关闭窗口" << activeWindow;
            }
        }
        previousWindow = activeWindow;
        activeWindow = name;
        if(activeWindow != "tray") {
            windowList[activeWindow].time = QDateTime::currentDateTime().currentSecsSinceEpoch();
            windowList[activeWindow].window->onWindowSelect();
            windowList[name].window->show();
            windowList[name].window->activateWindow();
            windowList[name].window->raise();
            windowList[activeWindow].window->receiveData(data1, data2);
        }
        KeyManager::instance()->onWindowChangeEnd(name);
    }
    if(!timer->isActive()) {
        int time = Config::getConfig<int>(Config::clear_interval);
        timer->start(time * 1000);
    }
}

void WindowManager::openWindow(QString name) {
    createWindow(name);
    previousWindow = activeWindow;
    activeWindow = name;
    windowList[name].window->show();

}

void WindowManager::checkWindow(){
    if(previousWindow != "tray")
        createWindow(previousWindow);
    QString nowWindow = previousWindow;
    previousWindow = activeWindow;
    activeWindow = nowWindow;
    if(nowWindow != "tray")
        windowList[nowWindow].window->show();
    closeWindow(previousWindow);
}

QString WindowManager::getNowWindow() {
    return activeWindow;
}

WindowBase* WindowManager::getWindow(QString name) {
    if(contains(name)) {
        return windowList[name].window;
    }
    return NULL;
}

void WindowManager::popWindow() {
    changeWindow(previousWindow);
}

bool WindowManager::contains(QString name) {
    bool ans = windowList.find(name) != windowList.end();
    return ans;
}

void WindowManager::createWindow(QString name) {
    if(name == "tray")
        return;
    if(windowList.find(name) == windowList.end()) {
        windowList[name] = createData(static_cast<WindowBase*>(
                                          Reflect::newInstance(name)));
        windowList[name].window->afterInitialize(name);
    }
}

void WindowManager::close() {
    for(auto iter=windowList.begin(); iter!=windowList.end(); iter++) {
        iter->window->onWindowClose();
        iter->window->deleteLater();
    }
    QApplication::quit();
}

WindowManager::WindowData WindowManager::createData(WindowBase *window) {
    WindowData data;
    data.time = QDateTime::currentDateTime().currentSecsSinceEpoch();
    data.window = window;
    return data;
}
