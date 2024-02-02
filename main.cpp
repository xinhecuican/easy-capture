#include "Setting/setting.h"
#include "Windows/capturewindow.h"
#include "Manager/WindowManager.h"

#include <QApplication>
#include<QTextCodec>
#include "Manager/config.h"
#include "Manager/KeyManager.h"
#include "Windows/paintwindow.h"
#include "JlCompress.h"
#include "Manager/update.h"
#include "Style_widget/tray.h"
#include <windows.h>
#include "MainFilter.h"
#include "Manager/history.h"
#include<dbghelp.h>
#include "Helper/debug.h"
#include "Helper/log.h"
#include<QPair>
#include "Manager/uimanager.h"
#include "Windows/scrollerwindow.h"
#include "Windows/pinwindow.h"
#ifndef QT_DEBUG
#include <QBreakpadHandler.h>
#include <QBreakpadHttpUploader.h>
#endif
//#pragma comment(lib,"E:/libs/Visual_Leak_Detector/lib/Win64/vld.lib")
//#ifdef _DEBUG
//#include "E:/libs/Visual_Leak_Detector/include/vld.h"
//#endif

void registerClasses();
void setupDebug();
void preLoad();
#ifdef TEST
#include "Tests/AllTests.h"
#include <QTest>
QTEST_MAIN(ConfigTest);
#else
int main(int argc, char *argv[]) {
    QApplication a(argc, argv);
    // 设置PWD
    QString applicationDirPathStr = QCoreApplication::applicationDirPath();
    QDir::setCurrent(applicationDirPathStr);

    if(argc == 3 && strncmp(argv[1], "buildTheme", 20) == 0) {
        UIManager::instance()->buildRelease(QString(argv[2]));
        return 0;
    }
    if(argc == 2 && strncmp(argv[1], "update", 10) == 0){
        Update::instance()->save();
        return 0;
    }

    setupDebug();
    preLoad();

    MainFilter* fliter = MainFilter::instance();
    a.installNativeEventFilter(fliter); // 捕获程序键盘事件
    a.setQuitOnLastWindowClosed(false);
    a.connect(&a, &QApplication::aboutToQuit, &a, [=]() {
        KeyManager::instance()->unRegisterAll();
        Update::instance()->onFinish();
        fliter->deleteLater();
    });
    qInfo() << "程序启动";
    int ans = a.exec();
    return ans;
}
#endif


void registerClasses() {
    Reflect::registerClass<Setting>();
    Reflect::registerClass<CaptureWindow>();
    Reflect::registerClass<PaintWindow>();
    Reflect::registerClass<ScrollerWindow>();
    Reflect::registerClass<PinWindow>();
}

void setupDebug(){
#ifndef QT_DEBUG
    QBreakpadInstance.setDumpPath(QLatin1String("crashs"));
    QBreakpadInstance.mainVersion = Update::now_version.get_version();
    logSysInit("Data/Temp/log.txt");
#endif
}

void preLoad(){
    Config::loadConfig();
    KeyManager::instance()->load();
    MString::load_from_file("Data/Languages/");
    registerClasses();
    QFont().defaultFamily(); // 提前加载字体文件，大约需要200-300ms
}
