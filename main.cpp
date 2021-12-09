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
#include "Style_widget/tray.h"
#include<windows.h>
#include "main_fliter.h"
#include "Paint/Widgets/history.h"

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
    Main_fliter* fliter = Main_fliter::instance();
    a.installEventFilter(fliter);//使用mainwindow上的eventfliter
    a.installNativeEventFilter(fliter);
    a.setQuitOnLastWindowClosed(false);
    MainWindow* main_window = new MainWindow();
    Window_manager::push_window("MainWindow", main_window);
    Window_manager::change_window("MainWindow");

    if(argc > 1 && argv[1] == MainWindow::tr("autoStart"))
    {
        Window_manager::hide_now();
    }
    a.connect(&a, &QApplication::aboutToQuit, &a, [=](){
        delete fliter;
        Key_manager::save();
        UnregisterHotKey((HWND)fliter->winId(), fliter->global_key_id);
        GlobalDeleteAtom( fliter->global_key_id );
    });
//    Update::instance()->save();
    int ans = a.exec();

    return ans;
}
