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
#include<dbghelp.h>
#include<windows.h>
#include "Helper/debug.h"
#include "Paint/Widgets/Layers/rect_layer.h"
#include "Paint/Widgets/Layers/text_layer.h"
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

    //这里弹出一个错误对话框并退出程序
    Debug::show_error_message(QObject::tr("Program crash\nSee crash.dmp for more information"));

    return EXCEPTION_EXECUTE_HANDLER;
}

void registerClasses();

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
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

void registerClasses()
{
    Reflect::registerClass<MainWindow>();
    Reflect::registerClass<Setting>();
    Reflect::registerClass<Capture_window>();
    Reflect::registerClass<Paint_window>();
    Reflect::registerClass<Rect_layer>();
    Reflect::registerClass<Text_layer>();
}
