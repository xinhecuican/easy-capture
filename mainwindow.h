#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include<QLabel>
#include "Base/Window_base.h"
#include<QtWidgets>
#include<QMessageBox>
#include "new_capture/capture_button_action.h"
#include "Manager/window_manager.h"
#include "Manager/config.h"
#include "windows.h"
#include "QAbstractNativeEventFilter"
QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public Window_base, public QAbstractNativeEventFilter
{
    Q_OBJECT

public:
    Q_INVOKABLE MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void closeEvent(QCloseEvent* event)override;
    bool eventFilter(QObject* o, QEvent* e)override;
    bool nativeEventFilter(const QByteArray &eventType, void *message, long *result)override;

public slots:
    void window_manager_thread();

private:
    Ui::MainWindow *ui;
    QLabel* m_label;
    QToolBar* toolbar;
    QToolButton* new_button;
    Capture_button_action* new_button_action;
    ATOM global_key_id;
};
#endif // MAINWINDOW_H
