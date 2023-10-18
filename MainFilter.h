#ifndef MAIN_FLITER_H
#define MAIN_FLITER_H

#include <QWidget>
#include "QAbstractNativeEventFilter"
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include "Style_widget/tray.h"
#include <QProcess>

/**
 * @brief The MainFilter class
 * 用于注册快捷键，全局快捷键和托盘，它会处理全局快捷键事件，并且将本进程的键盘事件传递给 @ref KeyManager
 * 此外还会检查更新及启用定时器来调用 @ref WindowManager::controlWindowClose()
 */
class MainFilter : public QWidget, public QAbstractNativeEventFilter {
    Q_OBJECT
public:
    MainFilter();
    ~MainFilter();
    static MainFilter* instance() {
        if(_instance == NULL) {
            _instance = new MainFilter();
        }
        return _instance;
    }
    bool eventFilter(QObject* o, QEvent* e)override;
    bool nativeEventFilter(const QByteArray &eventType, void *message, long *result)override;
    void stopTimer();
    void startTimer();
    bool isTimerRun();
public slots:
    void windowMnaagerThread();
private:
    void setTrayContextMenu();
    void checkCrash();
    Tray* tray;
    QTimer* timer;
    static MainFilter* _instance;
    QProcess* crashUploadProcess;
};

#endif // MAIN_FLITER_H
