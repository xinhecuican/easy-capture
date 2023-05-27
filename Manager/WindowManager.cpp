#include "WindowManager.h"
#include <QDebug>
#include "Helper/debug.h"
#include "config.h"
#include<QApplication>
#include<QThread>
#include "KeyManager.h"
#include "MainFilter.h"

QMap<QString, WindowManager::WindowData> WindowManager::windowList =
    QMap<QString, WindowManager::WindowData>();
QString WindowManager::activeWindow = "tray";
QString WindowManager::previousWindow = NULL;
bool WindowManager::currentHidden = false;
bool WindowManager::previousHidden = false;

WindowManager::WindowManager() {}

//延时删除，加快窗口间的切换速度
void WindowManager::controlWindowClose() {
    int time = Config::getConfig<int>(Config::clear_interval);
    qint64 current_time = QDateTime::currentDateTime().currentSecsSinceEpoch();
    QList<WindowBase*> temp_list = QList<WindowBase*>();
    // 如果是设置则不销毁窗口，否则设置完成后绘图窗口可能会重置
    if(activeWindow == "Setting")return;
    qInfo() << activeWindow << endl;
    for(auto iter=windowList.begin(); iter!=windowList.end();) {
        if(current_time-iter.value().time >= time&&
                (currentHidden || iter.key() != activeWindow)) {
            temp_list.append(iter.value().window);
            qInfo() << "delete " << iter.key();
            KeyManager::onWindowClose(iter.key());
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
        MainFilter::instance()->stopTimer();
    }
}

void WindowManager::closeWindow(QString name) {
    if(name == activeWindow) {
        qWarning("未关闭窗口调用closeWindow");
    } else if(windowList.find(name) != windowList.end()) {
        windowList[name].window->onWindowClose();
        windowList[name].window->deleteLater();
        windowList.remove(name);
        qInfo() << "关闭窗口" << name;
    }

}

void WindowManager::pushWindow(QString name, WindowBase *widget) {
    windowList[name] = createData(widget);
}

void WindowManager::changeWindow(QString name) {
    qInfo() << name << " " << activeWindow;
    createWindow(name);
    if(activeWindow != name) {
        KeyManager::onWindowChangeBegin(activeWindow, name);
        if(name == "tray") {
            windowList[activeWindow].window->setWindowFlag(Qt::WindowSystemMenuHint, false);
        }
        if(activeWindow != "tray" && windowList.contains(activeWindow)) {
            windowList[activeWindow].time = QDateTime::currentDateTime().currentSecsSinceEpoch();
            windowList[activeWindow].window->hide();
            windowList[activeWindow].window->onWindowCancel();
        }
        previousWindow = activeWindow;
        activeWindow = name;
        previousHidden = currentHidden;
        currentHidden = false;
        if(activeWindow != "tray") {
            windowList[activeWindow].time = QDateTime::currentDateTime().currentSecsSinceEpoch();
            windowList[activeWindow].window->onWindowSelect();
            windowList[name].window->show();
        }
        KeyManager::onWindowChangeEnd();
    }
    if(!MainFilter::instance()->isTimerRun()) {
        MainFilter::instance()->startTimer();
    }
}

void WindowManager::openWindow(QString name) {
    createWindow(name);
    previousWindow = activeWindow;
    activeWindow = name;
    windowList[name].window->show();

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
    if(previousHidden) {
        previousHidden = currentHidden;
        hideNow();
        QString temp_window = activeWindow;
        activeWindow = previousWindow;
        previousWindow = temp_window;
        return;
    }
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

void WindowManager::showNow() {
    createWindow(activeWindow);
    currentHidden = false;
    windowList[activeWindow].window->show();
    windowList[activeWindow].window->setWindowFlag(Qt::WindowSystemMenuHint, true);
    if(!MainFilter::instance()->isTimerRun()) {
        MainFilter::instance()->startTimer();
    }
}

void WindowManager::hideNow() {
    currentHidden = true;
    windowList[activeWindow].window->hide();
    windowList[activeWindow].window->setWindowFlag(Qt::WindowSystemMenuHint, false);
}
