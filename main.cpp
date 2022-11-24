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
#include<QPair>
//#ifdef QT_DEBUG
//#pragma comment(lib, "C:/usr/software/Visual_Leak_Detector/lib/Win64/vld.lib")
//#include "vld.h"
//#endif
//程式异常捕获
LONG ApplicationCrashHandler(EXCEPTION_POINTERS *pException)
{
    //创建 Dump 文件
    HANDLE hDumpFile = CreateFile(L"crash.dmp", GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hDumpFile != INVALID_HANDLE_VALUE)
    {
        //Dump信息
        MINIDUMP_EXCEPTION_INFORMATION dumpInfo;
        dumpInfo.ExceptionPointers = pException;
        dumpInfo.ThreadId = GetCurrentThreadId();
        dumpInfo.ClientPointers = TRUE;

        //写入Dump文件内容
        MiniDumpWriteDump(GetCurrentProcess(), GetCurrentProcessId(), hDumpFile, MiniDumpNormal, &dumpInfo, NULL, NULL);
    }
#ifdef QT_NO_DEBUG
    QString version = Update::now_version.get_version();
    QString cmd = "CrashHandler.exe " + version;
    QProcess::startDetached(cmd.toStdString().c_str());
#else
    //这里弹出一个错误对话框并退出程序
    Debug::show_error_message(QObject::tr("Program crash\nSee crash.dmp for more information"));
#endif
    abort();
    return EXCEPTION_EXECUTE_HANDLER;
}

void terminateHandler()
{
    //创建 Dump 文件
    HANDLE hDumpFile = CreateFile(L"crash.dmp", GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hDumpFile != INVALID_HANDLE_VALUE)
    {
        //Dump信息
        MINIDUMP_EXCEPTION_INFORMATION dumpInfo;
        dumpInfo.ThreadId = GetCurrentThreadId();
        dumpInfo.ClientPointers = TRUE;

        //写入Dump文件内容
        MiniDumpWriteDump(GetCurrentProcess(), GetCurrentProcessId(), hDumpFile, MiniDumpNormal, &dumpInfo, NULL, NULL);
    }

#ifdef QT_NO_DEBUG
    QString version = Update::now_version.get_version();
    QString cmd = "CrashHandler.exe " + version;
    QProcess::startDetached(cmd.toStdString().c_str());
#else
    //这里弹出一个错误对话框并退出程序
    Debug::show_error_message(QObject::tr("Program crash\nSee crash.dmp for more information"));
#endif
    abort();
}

void registerClasses();

#ifdef TEST
#include "Tests/AllTests.h"
#include <QTest>
QTEST_MAIN(ConfigTest);
#else
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
#ifdef QT_NO_DEBUG
    set_new_handler(terminateHandler);
    set_terminate(terminateHandler);
    _set_purecall_handler(terminateHandler);
#endif
    SetUnhandledExceptionFilter((LPTOP_LEVEL_EXCEPTION_FILTER)ApplicationCrashHandler);//注冊异常捕获函数
    QString applicationDirPathStr = QCoreApplication::applicationDirPath();
    QDir::setCurrent(applicationDirPathStr);
    Config::load_config();
    Key_manager::load();
    registerClasses();
    Main_fliter* fliter = Main_fliter::instance();
    a.installEventFilter(fliter);//使用mainwindow上的eventfliter
    a.installNativeEventFilter(fliter);
    a.setQuitOnLastWindowClosed(false);
//    MainWindow* main_window = new MainWindow();
//    Window_manager::push_window("MainWindow", main_window);
//    Window_manager::change_window("MainWindow");

//    if(argc > 1 && argv[1] == MainWindow::tr("autoStart"))
//    {
//        Window_manager::hide_now();
//    }
    a.connect(&a, &QApplication::aboutToQuit, &a, [=](){
        Key_manager::save();
        UnregisterHotKey((HWND)fliter->winId(), fliter->global_key_id);
        UnregisterHotKey((HWND)fliter->winId(), fliter->global_capture_id);
        GlobalDeleteAtom( fliter->global_key_id );
        GlobalDeleteAtom(fliter->global_capture_id);
        Update::instance()->onFinish();
        delete Config::instance();
        fliter->deleteLater();
    });
//    Update::instance()->save();
    int ans = a.exec();

    return ans;
}
#endif


void registerClasses()
{
    Reflect::registerClass<MainWindow>();
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
