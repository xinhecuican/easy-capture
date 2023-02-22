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
#include <windows.h>
#include "main_fliter.h"
#include "Paint/Widgets/history.h"
#include<dbghelp.h>
#include "Helper/debug.h"
#include "Paint/Widgets/Layers/rect_layer.h"
#include "Paint/Widgets/Layers/text_layer.h"
#include "Paint/Widgets/Layers/picture_layer.h"
#include "Paint/Widgets/Layers/paint_layer.h"
#include "Paint/Widgets/Layers/shapelayer.h"
#include "Paint/Widgets/Layers/blurlayer.h"
#include "Paint/Widgets/Layers/arrowlayer.h"
#include "new_capture/Widgets/cliplayer.h"
#include "Helper/dump.h"
#include "Helper/log.h"
#include<QPair>
#ifndef QT_DEBUG
#include <QBreakpadHandler.h>
#include <QBreakpadHttpUploader.h>
#endif
//#ifdef QT_DEBUG
//#pragma comment(lib, "C:/usr/software/Visual_Leak_Detector/lib/Win64/vld.lib")
//#include "vld.h"
//#endif

void registerClasses();
#ifdef TEST
#include "Tests/AllTests.h"
#include <QTest>
QTEST_MAIN(ConfigTest);
#else
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    // 设置PWD
    QString applicationDirPathStr = QCoreApplication::applicationDirPath();
    QDir::setCurrent(applicationDirPathStr);
#ifndef QT_DEBUG
    QBreakpadInstance.setDumpPath(QLatin1String("crashs"));
    QBreakpadInstance.mainVersion = Update::now_version.get_version();
    logSysInit("Data/Temp/log.txt");
#endif
    Config::load_config();
    Key_manager::load();
    MString::load_from_file("Data/Languages/");
    registerClasses();

    Main_fliter* fliter = Main_fliter::instance();
    a.installEventFilter(fliter);//捕获全局键盘事件
    a.installNativeEventFilter(fliter); // 捕获程序键盘事件
    a.setQuitOnLastWindowClosed(false);
//    MainWindow* main_window = new MainWindow();
//    Window_manager::push_window("MainWindow", main_window);
//    Window_manager::change_window("MainWindow");

//    if(argc > 1 && argv[1] == MainWindow::tr("autoStart"))
//    {
//        Window_manager::hide_now();
//    }
    a.connect(&a, &QApplication::aboutToQuit, &a, [=](){
//        UnregisterHotKey((HWND)fliter->winId(), fliter->global_key_id);
//        UnregisterHotKey((HWND)fliter->winId(), fliter->global_capture_id);
//        GlobalDeleteAtom( fliter->global_key_id );
//        GlobalDeleteAtom(fliter->global_capture_id);
        Key_manager::unRegisterAll();
        Update::instance()->onFinish();
        fliter->deleteLater();
    });
//    Update::instance()->save();
    int ans = a.exec();

    return ans;
}
#endif


void registerClasses()
{
    Reflect::registerClass<Setting>();
    Reflect::registerClass<Capture_window>();
    Reflect::registerClass<Paint_window>();
    Reflect::registerClass<Rect_layer>();
    Reflect::registerClass<Text_layer>();
    Reflect::registerClass<Picture_layer>();
    Reflect::registerClass<Paint_layer>();
    Reflect::registerClass<ShapeLayer>();
    Reflect::registerClass<BlurLayer>();
    Reflect::registerClass<ArrowLayer>();
    Reflect::registerClass<ClipRegion>();
}
