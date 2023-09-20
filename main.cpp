#include "Setting/setting.h"
#include "new_capture/CaptureWindow.h"
#include "Manager/WindowManager.h"

#include <QApplication>
#include<QTextCodec>
#include "Manager/config.h"
#include "Manager/KeyManager.h"
#include "Paint/PaintWindow.h"
#include "JlCompress.h"
#include "Manager/update.h"
#include "Style_widget/tray.h"
#include <windows.h>
#include "MainFilter.h"
#include "Paint/Widgets/history.h"
#include<dbghelp.h>
#include "Helper/debug.h"
#include "Paint/Widgets/Layers/rect_layer.h"
#include "Paint/Widgets/Layers/text_layer.h"
#include "Paint/Widgets/Layers/picture_layer.h"
#include "Paint/Widgets/Layers/paintlayer.h"
#include "Paint/Widgets/Layers/shapelayer.h"
#include "Paint/Widgets/Layers/blurlayer.h"
#include "Paint/Widgets/Layers/arrowlayer.h"
#include "new_capture/Widgets/cliplayer.h"
#include "Helper/log.h"
#include<QPair>
#include "Manager/uimanager.h"
#include "Paint/Widgets/Panels/flow_edit_panel.h"
#include "Windows/scrollerwindow.h"
#include "Windows/pinwindow.h"
#ifndef QT_DEBUG
#include <QBreakpadHandler.h>
#include <QBreakpadHttpUploader.h>
#endif
//#ifdef QT_DEBUG
//#pragma comment(lib, "C:/usr/software/Visual_Leak_Detector/lib/Win64/vld.lib")
//#include "vld.h"
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
    a.installEventFilter(fliter);//捕获全局键盘事件
    a.installNativeEventFilter(fliter); // 捕获程序键盘事件
    a.setQuitOnLastWindowClosed(false);
    a.connect(&a, &QApplication::aboutToQuit, &a, [=]() {
        KeyManager::unRegisterAll();
        Update::instance()->onFinish();
        fliter->deleteLater();
    });
    Flow_edit_panel::instance();// 初始化时间过长，提前初始化一次
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
    Reflect::registerClass<RectLayer>();
    Reflect::registerClass<Text_layer>();
    Reflect::registerClass<Picture_layer>();
    Reflect::registerClass< PaintLayer>();
    Reflect::registerClass<ShapeLayer>();
    Reflect::registerClass<BlurLayer>();
    Reflect::registerClass<ArrowLayer>();
    Reflect::registerClass<ClipRegion>();
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
    KeyManager::load();
    KeyManager::registerGlobalKey("awake_capture");
    KeyManager::registerGlobalKey("fullscreen_capture");
    MString::load_from_file("Data/Languages/");
    registerClasses();
}
