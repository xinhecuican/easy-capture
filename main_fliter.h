#ifndef MAIN_FLITER_H
#define MAIN_FLITER_H

#include <QWidget>
#include "QAbstractNativeEventFilter"
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include "Style_widget/tray.h"
#include <QProcess>

class Main_fliter : public QWidget, public QAbstractNativeEventFilter
{
    Q_OBJECT
public:
    Main_fliter();
    ~Main_fliter();
    static Main_fliter* instance()
    {
        if(_instance == NULL)
        {
            _instance = new Main_fliter();
        }
        return _instance;
    }
    bool eventFilter(QObject* o, QEvent* e)override;
    bool nativeEventFilter(const QByteArray &eventType, void *message, long *result)override;
    void stop_timer();
    void start_timer();
    bool is_timer_run();
    ATOM global_key_id;
    ATOM global_capture_id;
public slots:
    void window_manager_thread();
private:
    void setTrayContextMenu();
    void checkCrash();
    Tray* tray;
    QTimer* timer;
    static Main_fliter* _instance;
    QProcess* crashUploadProcess;
};

#endif // MAIN_FLITER_H
