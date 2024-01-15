#ifndef WINDOW_MANAGER_H
#define WINDOW_MANAGER_H
#include<map>
#include<iostream>
#include<QWidget>
#include "../Base/WindowBase.h"

#include<QMutex>
#include<QTime>
#include<QDateTime>
#include "../Helper/Template.h"
#include <QTimer>
using namespace std;

/**
 * @brief The WindowManager class
 * 用来管理窗口的启动，切换和销毁。窗体必须继承@ref WindowBase 并且需要调用@ref Reflect::registerClass() 注册窗体
 */
class WindowManager : public QObject{
    Q_OBJECT
    DECLARE_INSTANCE(WindowManager)
public:
    friend class MainFilter;
    WindowManager();
    void pushWindow(QString name, WindowBase* widget);
    /**
     * @brief changeWindow 切换到某个窗口，如果窗口不存在则创建
     * @param name 窗体的类名称
     */
    void changeWindow(QString name, QVariant data1=QVariant(), QVariant data2=QVariant());
    /**
     * @brief openWindow 开启窗口，原窗口不关闭
     * @param name
     * @see checkWindow()
     */
    void openWindow(QString name);
    /**
     * @brief checkWindow 切换回原窗口，并关闭当前窗口，切换过去的窗口并未关闭
     * @see openWindow()
     */
    void checkWindow();
    QString getNowWindow();
    void popWindow();
    bool contains(QString name);
    WindowBase* getWindow(QString name);
    void close();
    /**
     * @brief controlWindowClose 控制窗体的销毁，可以通过设置 clear_interval 控制销毁时间
     */
    void controlWindowClose();
    void closeWindow(QString name);
    void createWindow(QString name);
private:
    struct WindowData {
        qint64 time;
        WindowBase* window;
    };
    QMap<QString, WindowData> windowList;
    QString activeWindow;
    QString previousWindow;
    QTimer* timer;
    WindowData createData(WindowBase* window);
};



#endif // WINDOW_MANAGER_H
