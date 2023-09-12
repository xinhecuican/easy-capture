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

SOURCES += \
    Helper/debug.cpp \
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
    Manager/uimanager.cpp \
    Manager/update.cpp \
	Paint/Data/fontdata.cpp \
    Paint/Data/scroll_button.cpp \
    Paint/Data/stretch_button.cpp \
    Paint/PaintWindow.cpp \
    Paint/Widgets/Layer_list/list_item.cpp \
    Paint/Widgets/Layer_list/list_widget.cpp \
    Paint/Widgets/Layers/LayerItems/expandbutton.cpp \
    Paint/Widgets/Layers/LayerItems/paintitem.cpp \
	Paint/Widgets/Layers/LayerItems/picitem.cpp \
    Paint/Widgets/Layers/LayerItems/scrollitem.cpp \
    Paint/Widgets/Layers/arrowlayer.cpp \
    Paint/Widgets/Layers/blurlayer.cpp \
	Paint/Widgets/Layers/paintlayer.cpp \
    Paint/Widgets/Layers/picture_layer.cpp \
    Paint/Widgets/Layers/rect_layer.cpp \
    Paint/Widgets/Layers/shapelayer.cpp \
    Paint/Widgets/Layers/text_layer.cpp \
    Paint/Widgets/PaintArea.cpp \
    Paint/Widgets/Panels/close_dialog.cpp \
    Paint/Widgets/Panels/color_selector.cpp \
    Paint/Widgets/Panels/flow_edit_panel.cpp \
    Paint/Widgets/Panels/paint_setting_panel.cpp \
    Paint/Widgets/Recorder_element/paint_record.cpp \
    Paint/Widgets/Recorder_element/paintdeleterecord.cpp \
    Paint/Widgets/Recorder_element/resize_record.cpp \
    Paint/Widgets/Recorder_element/shaperecord.cpp \
    Paint/Widgets/history.cpp \
    Paint/Widgets/recorder.cpp \
    Paint/Widgets/style_manager.cpp \
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
    Style_widget/framelesshelper.cpp \
    Style_widget/rotate_view.cpp \
    Style_widget/spacer.cpp \
    Style_widget/titlebar.cpp \
    Style_widget/tray.cpp \
    Tests/configtest.cpp \
    Tests/testwidget.cpp \
	Windows/scrollerwindow.cpp \
    main.cpp \
    new_capture/CaptureWindow.cpp \
    new_capture/Widgets/Scroll_handler/Scroll_handle_global.cpp \
    new_capture/Widgets/Scroll_handler/dispatcher_worker.cpp \
    new_capture/Widgets/Scroll_handler/scroll_dispatcher.cpp \
    new_capture/Widgets/Scroll_handler/scroll_handle.cpp \
    new_capture/Widgets/Scroll_handler/scroll_worker.cpp \
    new_capture/Widgets/VideoHandler/videocapture.cpp \
    new_capture/Widgets/attributetoolbar.cpp \
    new_capture/Widgets/cliplayer.cpp \
    new_capture/Widgets/clipregion.cpp \
	new_capture/Widgets/colorpicker.cpp \
    new_capture/Widgets/masklayer.cpp \
    new_capture/Widgets/mosicsample.cpp \
    new_capture/Widgets/videotoolbar.cpp \
    new_capture/Window_fliter.cpp

HEADERS += \
    Base/MPool.h \
    Base/RecordElement.h \
    Base/Serializable.h \
    Base/TabBase.h \
    Base/WindowBase.h \
    Helper/EnumReflect.h \
    Helper/GraphicsViewPatch.h \
    Helper/Reflect.h \
    Helper/Serialize.h \
    Helper/common.h \
    Helper/debug.h \
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
	Manager/uimanager.h \
    Manager/update.h \
    Paint/Data/Common.h \
    Paint/Data/History_data.h \
    Paint/Data/Ilayer.h \
	Paint/Data/fontdata.h \
	Paint/Data/paint_data.h \
    Paint/Data/scroll_button.h \
    Paint/Data/stretch_button.h \
    Paint/PaintWindow.h \
    Paint/Widgets/Layer_list/list_item.h \
    Paint/Widgets/Layer_list/list_widget.h \
    Paint/Widgets/Layers/LayerItems/expandbutton.h \
    Paint/Widgets/Layers/LayerItems/paintitem.h \
	Paint/Widgets/Layers/LayerItems/picitem.h \
    Paint/Widgets/Layers/LayerItems/scrollitem.h \
    Paint/Widgets/Layers/arrowlayer.h \
    Paint/Widgets/Layers/baselayer.h \
    Paint/Widgets/Layers/blurlayer.h \
	Paint/Widgets/Layers/paintlayer.h \
    Paint/Widgets/Layers/picture_layer.h \
    Paint/Widgets/Layers/rect_layer.h \
    Paint/Widgets/Layers/shapelayer.h \
    Paint/Widgets/Layers/text_layer.h \
    Paint/Widgets/Panels/close_dialog.h \
    Paint/Widgets/Panels/color_selector.h \
    Paint/Widgets/Panels/flow_edit_panel.h \
    Paint/Widgets/Panels/paint_setting_panel.h \
    Paint/Widgets/Recorder_element/paint_record.h \
    Paint/Widgets/Recorder_element/paintdeleterecord.h \
    Paint/Widgets/Recorder_element/resize_record.h \
    Paint/Widgets/Recorder_element/shaperecord.h \
    Paint/Widgets/history.h \
	Paint/Widgets/PaintArea.h \
    Paint/Widgets/recorder.h \
    Paint/Widgets/style_manager.h \
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
    Style_widget/framelesshelper.h \
    Style_widget/rotate_view.h \
    Style_widget/spacer.h \
    Style_widget/titlebar.h \
    Style_widget/tray.h \
    Tests/AllTests.h \
    Tests/configtest.h \
    Tests/testwidget.h \
	Windows/scrollerwindow.h \
    new_capture/CaptureWindow.h \
    new_capture/Widgets/Scroll_handler/Scroll_handler_global.h \
    new_capture/Widgets/Scroll_handler/dispatcher_worker.h \
    new_capture/Widgets/Scroll_handler/scroll_dispatcher.h \
    new_capture/Widgets/Scroll_handler/scroll_handle.h \
    new_capture/Widgets/Scroll_handler/scroll_worker.h \
    new_capture/Widgets/VideoHandler/videocapture.h \
    new_capture/Widgets/attributetoolbar.h \
    new_capture/Widgets/cliplayer.h \
    new_capture/Widgets/clipregion.h \
	new_capture/Widgets/colorpicker.h \
    new_capture/Widgets/masklayer.h \
    new_capture/Widgets/mosicsample.h \
    new_capture/Widgets/videotoolbar.h \
    new_capture/window_fliter.h

FORMS += \
	Setting/setting.ui \
    Style_widget/ColorWidget.ui \
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

win32:CONFIG(release, debug|release): include($$PWD/../qBreakpad/qBreakpad.pri)
win32:CONFIG(release, debug|release): QMAKE_LIBDIR += $$PWD/../qBreakpad/handler
win32:CONFIG(release, debug|release): LIBS += -lqBreakpad

#win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../build-Hook-Desktop_Qt_5_14_2_MinGW_64_bit-Release/release/ -lHook
#else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../build-Hook-Desktop_Qt_5_14_2_MinGW_64_bit-Debug/debug/ -lHook
#else:unix: LIBS += -L$$PWD/../build-Hook-Desktop_Qt_5_14_2_MinGW_64_bit-Debug/ -lHook

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../build-Hook-Desktop_Qt_5_14_2_MSVC2017_64bit-Release/release/ -lHook
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../build-Hook-Desktop_Qt_5_14_2_MSVC2017_64bit-Debug/debug/ -lHook
else:unix: LIBS += -L$$PWD/../build-Hook-MSVC-Debug/ -lHook

INCLUDEPATH += $$PWD/../HOOK
DEPENDPATH += $$PWD/../HOOK

#win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../build-quazip-Desktop_Qt_5_14_2_MinGW_64_bit-Release/quazip/release/ -lquazip
#else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../build-quazip-Desktop_Qt_5_14_2_MinGW_64_bit-Debug/quazip/debug/ -lquazipd
#else:unix: LIBS += -L$$PWD/../build-quazip-Desktop_Qt_5_14_2_MinGW_64_bit-Debug/quazip/ -lquazipd

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../build-quazip-Desktop_Qt_5_14_2_MSVC2017_64bit-Release/quazip/release/ -lquazip
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../build-quazip-Desktop_Qt_5_14_2_MSVC2017_64bit-Debug/quazip/debug/ -lquazipd
else:unix: LIBS += -L$$PWD/../build-quazip-Desktop_Qt_5_14_2_MSVC2017_64bit-Debug/ -lquazipd

INCLUDEPATH += $$PWD/../quazip-0.7.3/quazip
DEPENDPATH += $$PWD/../quazip-0.7.3/quazip

LIBS += -lGdi32 -loleacc -lDbgHelp
LIBS += \
    -ldwmapi -lOle32 -lksuser -lUser32

RC_FILE = logo.rc

INCLUDEPATH += $$PWD/../opencv/opencv4.5.1/include \
    $$PWD/../opencv/opencv4.5.1/include/opencv2

#LIBS += F:/capture/opencv/opencv4.5.1/x64/mingw/lib/libopencv_*
win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../opencv/opencv4.5.1-msvc/x64/vc16/lib\
-lopencv_xfeatures2d451 -lopencv_imgproc451 -lopencv_imgcodecs451\
-lopencv_core451 -lopencv_flann451 -lopencv_calib3d451 -lopencv_features2d451
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../opencv/opencv4.5.1-msvc/x64/vc16/lib\
-lopencv_xfeatures2d451d -lopencv_imgproc451d -lopencv_imgcodecs451d\
-lopencv_core451d -lopencv_flann451d -lopencv_calib3d451d -lopencv_features2d451d -lopencv_highgui451d
else:unix: LIBS += -L$$PWD/../opencv/opencv4.5.1-msvc/x64/vc16/lib\
-lopencv_xfeatures2d451d -lopencv_imgproc451d -lopencv_imgcodecs451d\
-lopencv_core451d -lopencv_flann451d -lopencv_calib3d451d -lopencv_features2d451d

msvc {
    QMAKE_CFLAGS += /utf-8
    QMAKE_CXXFLAGS += /utf-8
}

DEPENDPATH += E:/libs/Visual_Leak_Detector/include
INCLUDEPATH += E:/libs/Visual_Leak_Detector/include
win32:CONFIG(release, debug|release): LIBS += -LE:/libs/Visual_Leak_Detector/lib/Win64 -lvld

DISTFILES += \
	Resource/toolbar.qss

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../build-ECVideoCapture-Desktop_Qt_5_14_2_MSVC2017_64bit-Release/release/ -lECVideoCapture
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../build-ECVideoCapture-Desktop_Qt_5_14_2_MSVC2017_64bit-Release/release/ -lECVideoCapture
else:unix: LIBS += -L$$PWD/../build-ECVideoCapture-Desktop_Qt_5_14_2_MSVC2017_64bit-Release/ -lECVideoCapture

INCLUDEPATH += $$PWD/../ECVideoCapture
DEPENDPATH += $$PWD/../ECVideoCapture
