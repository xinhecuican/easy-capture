#ifndef WINDOW_MANAGER_H
#define WINDOW_MANAGER_H
#include<map>
#include<iostream>
#include<QWidget>
#include "Base/WindowBase.h"

#include<QMutex>
#include<QTime>
#include<QDateTime>
using namespace std;

/**
 * @brief The WindowManager class
 * 用来管理窗口的启动，切换和销毁。窗体必须继承@ref WindowBase 并且需要调用@ref Reflect::registerClass() 注册窗体
 */
class WindowManager {
public:
    friend class MainFilter;
    WindowManager();
    static void pushWindow(QString name, WindowBase* widget);

    /**
     * @brief changeWindow 切换到某个窗口，如果窗口不存在则创建
     * @param name 窗体的类名称
     */
    static void changeWindow(QString name);
    static void openWindow(QString name);
    static QString getNowWindow();
    static void popWindow();
    static bool contains(QString name);
    static WindowBase* getWindow(QString name);
    static void close();
    /**
     * @brief controlWindowClose 控制窗体的销毁，可以通过设置 clear_interval 控制销毁时间
     */
    static void controlWindowClose();
    static void closeWindow(QString name);
    static void showNow();
    static void hideNow();

    /**
     * @brief hideToMain
     * @deprecated
     */
    static void hideToMain();
    static void createWindow(QString name);
private:
    struct WindowData {
        qint64 time;
        WindowBase* window;
    };
    static QMap<QString, WindowData> windowList;
    static QString activeWindow;
    static QString previousWindow;
    static WindowData createData(WindowBase* window);
    static bool currentHidden;
    static bool previousHidden;
};



#endif // WINDOW_MANAGER_H
