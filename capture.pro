QT       += core gui xml network multimedia svg

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0
QMAKE_CXXFLAGS_RELEASE = $$QMAKE_CFLAGS_RELEASE_WITH_DEBUGINFO
QMAKE_LFLAGS_RELEASE = $$QMAKE_LFLAGS_RELEASE_WITH_DEBUGINFO

SOURCES += \
	GraphicsScene/Container/blurlayercontainer.cpp \
	GraphicsScene/Container/geolayercontainer.cpp \
	GraphicsScene/Container/layercontainer.cpp \
	GraphicsScene/Container/masklayercontainer.cpp \
	GraphicsScene/Container/paintlayercontainer.cpp \
	GraphicsScene/Container/textlayercontainer.cpp \
	GraphicsScene/Container/videolayercontainer.cpp \
	GraphicsScene/Layer/arrowlayer.cpp \
    GraphicsScene/Layer/backgroundlayer.cpp \
	GraphicsScene/Layer/blurlayer.cpp \
	GraphicsScene/Layer/grabcutlayer.cpp \
	GraphicsScene/Layer/layerbase.cpp \
	GraphicsScene/Layer/maskcolorlayer.cpp \
	GraphicsScene/Layer/masklayer.cpp \
	GraphicsScene/Layer/paintlayer.cpp \
	GraphicsScene/Layer/piclayer.cpp \
	GraphicsScene/Layer/rectlayer.cpp \
	GraphicsScene/Layer/rootlayer.cpp \
	GraphicsScene/Layer/textlayer.cpp \
	GraphicsScene/Recorder/layerrecord.cpp \
	GraphicsScene/Recorder/recorder.cpp \
	GraphicsScene/Widgets/attributetoolbar.cpp \
	GraphicsScene/Widgets/clipregion.cpp \
	GraphicsScene/Widgets/colorpicker.cpp \
	GraphicsScene/Widgets/defaulttoolbar.cpp \
	GraphicsScene/Widgets/expandbutton.cpp \
	GraphicsScene/Widgets/grabcuthandler.cpp \
	GraphicsScene/Widgets/layertoolbar.cpp \
	GraphicsScene/Widgets/painttoolbar.cpp \
	GraphicsScene/Widgets/scrollitem.cpp \
	GraphicsScene/Widgets/textitem.cpp \
	GraphicsScene/ilayercontrol.cpp \
	GraphicsScene/layermanager.cpp \
	GraphicsScene/paintarea.cpp \
    Helper/debug.cpp \
	Helper/dxgimanager.cpp \
	Helper/imagehelper.cpp \
    Helper/log.cpp \
    Helper/math.cpp \
    Helper/mstring.cpp \
    MainFilter.cpp \
	Manager/Data/UpdateData.cpp \
	Manager/Data/uiglobalparser.cpp \
	Manager/Data/uiparser.cpp \
    Manager/Data/update_dialog.cpp \
    Manager/Data/updatedownloader.cpp \
    Manager/KeyManager.cpp \
    Manager/WindowManager.cpp \
    Manager/config.cpp \
	Manager/history.cpp \
    Manager/uimanager.cpp \
    Manager/update.cpp \
	Setting/Widgets/BoolOption.cpp \
	Setting/Widgets/ComboOption.cpp \
	Setting/Widgets/GlobalKeyOption.cpp \
	Setting/Widgets/KeyOption.cpp \
	Setting/Widgets/NumOption.cpp \
	Setting/Widgets/TabWidget.cpp \
	Setting/Widgets/filechooser.cpp \
	Setting/Widgets/listwrapper.cpp \
	Setting/setting.cpp \
    Style_widget/bubbletipswidget.cpp \
    Style_widget/colorwidget.cpp \
	Style_widget/fadelabel.cpp \
    Style_widget/framelesshelper.cpp \
    Style_widget/rotate_view.cpp \
    Style_widget/spacer.cpp \
    Style_widget/titlebar.cpp \
    Style_widget/tray.cpp \
    Tests/configtest.cpp \
    Tests/testwidget.cpp \
	Windows/Widgets/pinwidget.cpp \
	Windows/Widgets/videocapture.cpp \
	Windows/capturewindow.cpp \
	Windows/paintwindow.cpp \
	Windows/pinwindow.cpp \
	Windows/scrollerwindow.cpp \
	Windows/windowfilter.cpp \
    main.cpp \
	Windows/ScrollHandler/Scroll_handle_global.cpp \
	Windows/ScrollHandler/dispatcher_worker.cpp \
	Windows/ScrollHandler/scroll_dispatcher.cpp \
	Windows/ScrollHandler/scroll_handle.cpp \
	Windows/ScrollHandler/scroll_worker.cpp

HEADERS += \
    Base/MPool.h \
    Base/RecordElement.h \
    Base/Serializable.h \
    Base/TabBase.h \
    Base/WindowBase.h \
	GraphicsScene/Container/blurlayercontainer.h \
	GraphicsScene/Container/geolayercontainer.h \
	GraphicsScene/Container/layercontainer.h \
	GraphicsScene/Container/masklayercontainer.h \
	GraphicsScene/Container/paintlayercontainer.h \
	GraphicsScene/Container/textlayercontainer.h \
	GraphicsScene/Container/videolayercontainer.h \
	GraphicsScene/Layer/arrowlayer.h \
    GraphicsScene/Layer/backgroundlayer.h \
	GraphicsScene/Layer/blurlayer.h \
	GraphicsScene/Layer/cliplayerbase.h \
	GraphicsScene/Layer/grabcutlayer.h \
	GraphicsScene/Layer/layerbase.h \
	GraphicsScene/Layer/maskcolorlayer.h \
	GraphicsScene/Layer/masklayer.h \
	GraphicsScene/Layer/paintlayer.h \
	GraphicsScene/Layer/piclayer.h \
	GraphicsScene/Layer/rectlayer.h \
	GraphicsScene/Layer/rootlayer.h \
	GraphicsScene/Layer/textlayer.h \
	GraphicsScene/Recorder/layerrecord.h \
	GraphicsScene/Recorder/recordbase.h \
	GraphicsScene/Recorder/recorder.h \
	GraphicsScene/Widgets/FontData.h \
	GraphicsScene/Widgets/PaintData.h \
	GraphicsScene/Widgets/attributetoolbar.h \
	GraphicsScene/Widgets/clipregion.h \
	GraphicsScene/Widgets/colorpicker.h \
	GraphicsScene/Widgets/defaulttoolbar.h \
	GraphicsScene/Widgets/expandbutton.h \
	GraphicsScene/Widgets/grabcuthandler.h \
	GraphicsScene/Widgets/layertoolbar.h \
	GraphicsScene/Widgets/painttoolbar.h \
	GraphicsScene/Widgets/scrollitem.h \
	GraphicsScene/Widgets/textitem.h \
	GraphicsScene/ilayercontrol.h \
	GraphicsScene/layermanager.h \
	GraphicsScene/paintarea.h \
    Helper/EnumReflect.h \
    Helper/GraphicsViewPatch.h \
	Helper/PaintHelper.h \
    Helper/Reflect.h \
    Helper/Serialize.h \
	Helper/Template.h \
    Helper/common.h \
    Helper/debug.h \
	Helper/dxgimanager.h \
	Helper/global.h \
	Helper/imagehelper.h \
    Helper/log.h \
    Helper/math.h \
    Helper/mstring.h \
    Helper/plist.h \
    MainFilter.h \
    Manager/Data/Reply_timeout.h \
	Manager/Data/UpdateData.h \
	Manager/Data/uiglobalparser.h \
	Manager/Data/uiparser.h \
    Manager/Data/update_dialog.h \
    Manager/Data/updatedownloader.h \
    Manager/IKeyListener.h \
    Manager/KeyManager.h \
    Manager/WindowManager.h \
    Manager/config.h \
	Manager/history.h \
	Manager/uimanager.h \
    Manager/update.h \
	Setting/Widgets/BoolOption.h \
	Setting/Widgets/ComboOption.h \
	Setting/Widgets/GlobalKeyOption.h \
	Setting/Widgets/KeyOption.h \
	Setting/Widgets/NumOption.h \
	Setting/Widgets/TabWidget.h \
	Setting/Widgets/filechooser.h \
	Setting/Widgets/listwrapper.h \
	Setting/setting.h \
    Style_widget/bubbletipswidget.h \
    Style_widget/colorwidget.h \
	Style_widget/fadelabel.h \
    Style_widget/framelesshelper.h \
    Style_widget/rotate_view.h \
    Style_widget/spacer.h \
    Style_widget/titlebar.h \
    Style_widget/tray.h \
    Tests/AllTests.h \
    Tests/configtest.h \
    Tests/testwidget.h \
	Windows/Widgets/pinwidget.h \
	Windows/Widgets/videocapture.h \
	Windows/capturewindow.h \
	Windows/paintwindow.h \
	Windows/pinwindow.h \
	Windows/scrollerwindow.h \
	Windows/windowfilter.h \
	Windows/ScrollHandler/Scroll_handler_global.h \
	Windows/ScrollHandler/dispatcher_worker.h \
	Windows/ScrollHandler/scroll_dispatcher.h \
	Windows/ScrollHandler/scroll_handle.h \
	Windows/ScrollHandler/scroll_worker.h

FORMS += \
	Setting/setting.ui \
    Style_widget/bubbletipswidget.ui \
    Tests/testwidget.ui

TRANSLATIONS += \
    capture_zh_CN.ts

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

#QMAKE_CXXFLAGS_RELEASE = $$QMAKE_CFLAGS_RELEASE_WITH_DEBUGINFO
#QMAKE_LFLAGS_RELEASE = $$QMAKE_LFLAGS_RELEASE_WITH_DEBUGINFO
DEFINES += QT_MESSAGELOGCONTEXT


RESOURCES += \
    Resource/Resources.qrc\

win32:CONFIG(release, debug|release):INCLUDEPATH += $$PWD/lib/qBreakpad/handler/
win32:CONFIG(release, debug|release):HEADERS += \
	$$PWD/lib/qBreakpad/handler/QBreakpadHandler.h \
	$$PWD/lib/qBreakpad/handler/QBreakpadHttpUploader.h
win32:CONFIG(release, debug|release):LIBS += \
	-L$$PWD/lib/qBreakpad/handler -lqBreakpad

#win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../build-Hook-Desktop_Qt_5_14_2_MinGW_64_bit-Release/release/ -lHook
#else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../build-Hook-Desktop_Qt_5_14_2_MinGW_64_bit-Debug/debug/ -lHook
#else:unix: LIBS += -L$$PWD/../build-Hook-Desktop_Qt_5_14_2_MinGW_64_bit-Debug/ -lHook

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/lib/Hook/lib -lHook
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/lib/Hook/lib -lHook
else:unix: LIBS += -L$$PWD/../build-Hook-MSVC-Debug/ -lHook

INCLUDEPATH += $$PWD/lib/Hook/include
DEPENDPATH += $$PWD/lib/Hook/include

#win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../build-quazip-Desktop_Qt_5_14_2_MinGW_64_bit-Release/quazip/release/ -lquazip
#else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../build-quazip-Desktop_Qt_5_14_2_MinGW_64_bit-Debug/quazip/debug/ -lquazipd
#else:unix: LIBS += -L$$PWD/../build-quazip-Desktop_Qt_5_14_2_MinGW_64_bit-Debug/quazip/ -lquazipd

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/lib/quazip/lib -lquazip
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/lib/quazip/lib -lquazipd
else:unix: LIBS += -L$$PWD/../build-quazip-Desktop_Qt_5_14_2_MSVC2017_64bit-Debug/ -lquazipd

INCLUDEPATH += $$PWD/lib/quazip/include
DEPENDPATH += $$PWD/lib/quazip/include

LIBS += -lGdi32 -loleacc -lDbgHelp
LIBS += \
	-ldwmapi -lOle32 -lksuser -lUser32 -lD3D11 -lDXGI

RC_FILE = logo.rc

INCLUDEPATH += $$PWD/lib/opencv/include \
	$$PWD/lib/opencv/include/opencv2

#LIBS += F:/capture/opencv/opencv4.5.1/x64/mingw/lib/libopencv_*
win32:CONFIG(release, debug|release): LIBS += -L$$PWD/lib/opencv/lib\
-lopencv_xfeatures2d451 -lopencv_imgproc451 -lopencv_imgcodecs451\
-lopencv_core451 -lopencv_flann451 -lopencv_calib3d451 -lopencv_features2d451
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/lib/opencv/lib\
-lopencv_xfeatures2d451d -lopencv_imgproc451d -lopencv_imgcodecs451d\
-lopencv_core451d -lopencv_flann451d -lopencv_calib3d451d -lopencv_features2d451d -lopencv_highgui451d
else:unix: LIBS += -L$$PWD/lib/opencv/lib\
-lopencv_xfeatures2d451d -lopencv_imgproc451d -lopencv_imgcodecs451d\
-lopencv_core451d -lopencv_flann451d -lopencv_calib3d451d -lopencv_features2d451d

msvc {
    QMAKE_CFLAGS += /utf-8
    QMAKE_CXXFLAGS += /utf-8
}

#win32{
#    CONFIG(debug, debug|release) {
#    INCLUDEPATH += E:/libs/Visual_Leak_Detector/include
#    DEPENDPATH += E:/libs/Visual_Leak_Detector/include
#    LIBS += -LE:/libs/Visual_Leak_Detector/lib/Win64 -lvld
#    }
#}

DISTFILES += \
	Resource/toolbar.qss

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/lib/ECVideoCapture/lib -lECVideoCapture
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/lib/ECVideoCapture/lib -lECVideoCapture
else:unix: LIBS += -L$$PWD/lib/ECVideoCapture/lib -lECVideoCapture

INCLUDEPATH += $$PWD/lib/ECVideoCapture/include
DEPENDPATH += $$PWD/lib/ECVideoCapture/include
