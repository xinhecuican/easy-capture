#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include<QLabel>
#include "Base/Window_base.h"
#include<QtWidgets>
#include<QMessageBox>
#include "Manager/window_manager.h"
#include "Manager/config.h"
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include "QAbstractNativeEventFilter"
QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public Window_base
{
    Q_OBJECT

public:
    Q_INVOKABLE MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void closeEvent(QCloseEvent* event)override;
    void changeEvent(QEvent* event)override;
    void load_key_event(QString name) override;

public slots:
    void onCaptureButtonEnter();

private:
    Ui::MainWindow *ui;
    QLabel* m_label;
    QToolBar* toolbar;
    QToolButton* new_button;
    ATOM global_key_id;
    static bool is_start;
    bool hiding;
};
#endif // MAINWINDOW_H
