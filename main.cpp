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
int main(int argc, char *argv[]) {
    QApplication a(argc, argv);
    // 设置PWD
    QString applicationDirPathStr = QCoreApplication::applicationDirPath();
    QDir::setCurrent(applicationDirPathStr);
#ifndef QT_DEBUG
    QBreakpadInstance.setDumpPath(QLatin1String("crashs"));
    QBreakpadInstance.mainVersion = Update::now_version.get_version();
    logSysInit("Data/Temp/log.txt");
#endif
    Config::loadConfig();
    KeyManager::load();
    KeyManager::registerGlobalKey("awake_capture");
    KeyManager::registerGlobalKey("fullscreen_capture");
    MString::load_from_file("Data/Languages/");
    registerClasses();

    MainFilter* fliter = MainFilter::instance();
    a.installEventFilter(fliter);//捕获全局键盘事件
    a.installNativeEventFilter(fliter); // 捕获程序键盘事件
    a.setQuitOnLastWindowClosed(false);
    a.connect(&a, &QApplication::aboutToQuit, &a, [=]() {
        KeyManager::unRegisterAll();
        Update::instance()->onFinish();
        fliter->deleteLater();
    });
//    Update::instance()->save();
    int ans = a.exec();

    return ans;
}
#endif


void registerClasses() {
    Reflect::registerClass<Setting>();
    Reflect::registerClass<CaptureWindow>();
    Reflect::registerClass<PaintWindow>();
    Reflect::registerClass<RectLayer>();
    Reflect::registerClass<Text_layer>();
    Reflect::registerClass<Picture_layer>();
    Reflect::registerClass< PaintLayer>();
    Reflect::registerClass<ShapeLayer>();
    Reflect::registerClass<BlurLayer>();
    Reflect::registerClass<ArrowLayer>();
    Reflect::registerClass<ClipRegion>();
}
