#ifndef MAIN_FLITER_H
#define MAIN_FLITER_H

#include <QWidget>
#include "QAbstractNativeEventFilter"
#define WIN32_LEAN_AND_MEAN
#ifdef Q_OS_WIN
#include <windows.h>
#endif
#include "Style_widget/tray.h"
#include <QProcess>
#include "Manager/Data/FuncData.h"

/**
 * @brief The MainFilter class
 * 用于注册快捷键，全局快捷键和托盘，它会处理全局快捷键事件，并且将本进程的键盘事件传递给@ref KeyManager
 * 此外还会检查更新及启用定时器来调用 @ref WindowManager::controlWindowClose()
 */
class MainFilter : public QWidget {
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
    void stopTimer();
    void startTimer();
    bool isTimerRun();

#ifdef Q_OS_WIN
    ATOM global_key_id;
    ATOM global_capture_id;
#endif
public slots:
    void windowMnaagerThread();
    void onGlobalKeyTriggered(QString funcName);
private:
    void setTrayContextMenu();
    void checkCrash();
    void initGlobalKeyFuncs();
    Tray* tray;
    QTimer* timer;
    static MainFilter* _instance;
    QProcess* crashUploadProcess;
    QHash<QString, std::function<void ()>> globalKeyFuncs;
};

#endif // MAIN_FLITER_H
