#include "mainwindow.h"
#include "Panel/setting.h"
#include "new_capture/capture_window.h"
#include "Manager/window_manager.h"

#include <QApplication>
#include<QTextCodec>
#include "Manager/config.h"

int main(int argc, char *argv[])
{
    Config::load_config();
    QApplication a(argc, argv);
    Reflect::registerClass<MainWindow>();
    Reflect::registerClass<Setting>();
    Reflect::registerClass<Capture_window>();
    Window_manager::change_window("MainWindow");
    a.installEventFilter(Window_manager::get_window("MainWindow"));

    return a.exec();
}
