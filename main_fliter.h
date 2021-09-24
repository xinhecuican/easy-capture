#ifndef MAIN_FLITER_H
#define MAIN_FLITER_H

#include <QWidget>
#include "QAbstractNativeEventFilter"
#include<windows.h>
#include "Style_widget/tray.h"

class Main_fliter : public QWidget, public QAbstractNativeEventFilter
{
    Q_OBJECT
public:
    Main_fliter();
    ~Main_fliter();
    bool eventFilter(QObject* o, QEvent* e)override;
    bool nativeEventFilter(const QByteArray &eventType, void *message, long *result)override;
    ATOM global_key_id;
public slots:
    void window_manager_thread();
private:
    Tray* tray;
};

#endif // MAIN_FLITER_H
