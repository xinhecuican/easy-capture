#include "mainwindow.h"
#include "Panel/setting.h"
#include "new_capture/capture_window.h"
#include "Manager/window_manager.h"

#include <QApplication>
#include<QTextCodec>
#include "Manager/config.h"
#include "Manager/key_manager.h"
#include "Paint/paint_window.h"

int main(int argc, char *argv[])
{
    Config::load_config();
    Key_manager::load();
    QApplication a(argc, argv);
    Reflect::registerClass<MainWindow>();
    Reflect::registerClass<Setting>();
    Reflect::registerClass<Capture_window>();
    Reflect::registerClass<Paint_window>();
    Window_manager::change_window("MainWindow");
    a.installEventFilter(Window_manager::create_and_get_window("MainWindow"));//使用mainwindow上的eventfliter

    return a.exec();
}
