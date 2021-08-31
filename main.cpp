#include "mainwindow.h"
#include "Panel/setting.h"
#include "new_capture/capture_window.h"
#include "Manager/window_manager.h"

#include <QApplication>
#include<QTextCodec>
#include "Manager/config.h"
#include "Manager/key_manager.h"
#include "Paint/paint_window.h"
#include "JlCompress.h"
#include "Manager/update.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QString applicationDirPathStr = QCoreApplication::applicationDirPath();
    QDir::setCurrent(applicationDirPathStr);
    Config::load_config();
    Key_manager::load();
    Reflect::registerClass<MainWindow>();
    Reflect::registerClass<Setting>();
    Reflect::registerClass<Capture_window>();
    Reflect::registerClass<Paint_window>();
    MainWindow* main_window = new MainWindow();
    Window_manager::push_window("MainWindow", main_window);
    Window_manager::change_window("MainWindow");
    a.installEventFilter(main_window);//使用mainwindow上的eventfliter
    a.installNativeEventFilter(main_window);
    QApplication::mouseButtons();

    if(argc > 1 && argv[1] == MainWindow::tr("autoStart"))
    {
        Window_manager::hide_now();
    }
    int ans = a.exec();

    //Update::instance()->save();
    return ans;
}
