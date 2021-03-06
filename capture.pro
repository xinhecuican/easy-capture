QT       += core gui xml network

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
    Helper/image_helper.cpp \
    Helper/math.cpp \
    Helper/mstring.cpp \
    Manager/Data/update_data.cpp \
    Manager/Data/update_dialog.cpp \
    Manager/config.cpp \
    Manager/key_manager.cpp \
    Manager/update.cpp \
    Manager/window_manager.cpp \
    Paint/Data/scroll_button.cpp \
    Paint/Data/stretch_button.cpp \
    Paint/Widgets/Layer_list/list_item.cpp \
    Paint/Widgets/Layer_list/list_widget.cpp \
    Paint/Widgets/Layers/LayerItems/expandbutton.cpp \
    Paint/Widgets/Layers/LayerItems/paintitem.cpp \
    Paint/Widgets/Layers/LayerItems/scrollitem.cpp \
    Paint/Widgets/Layers/arrowlayer.cpp \
    Paint/Widgets/Layers/blurlayer.cpp \
    Paint/Widgets/Layers/paint_layer.cpp \
    Paint/Widgets/Layers/picture_layer.cpp \
    Paint/Widgets/Layers/rect_layer.cpp \
    Paint/Widgets/Layers/shapelayer.cpp \
    Paint/Widgets/Layers/text_edit.cpp \
    Paint/Widgets/Layers/text_layer.cpp \
    Paint/Widgets/Panels/close_dialog.cpp \
    Paint/Widgets/Panels/color_selector.cpp \
    Paint/Widgets/Panels/flow_edit_panel.cpp \
    Paint/Widgets/Panels/paint_setting_panel.cpp \
    Paint/Widgets/Recorder_element/paint_record.cpp \
    Paint/Widgets/Recorder_element/paintdeleterecord.cpp \
    Paint/Widgets/Recorder_element/resize_record.cpp \
    Paint/Widgets/history.cpp \
    Paint/Widgets/paint_area.cpp \
    Paint/Widgets/recorder.cpp \
    Paint/Widgets/style_manager.cpp \
    Paint/paint_window.cpp \
    Panel/Widgets/combo_tab.cpp \
    Panel/Widgets/filechooser.cpp \
    Panel/Widgets/key_tab.cpp \
    Panel/Widgets/num_tab.cpp \
    Panel/Widgets/tab_widget.cpp \
    Panel/Widgets/bool_tab.cpp \
    Panel/setting.cpp \
    Style_widget/framelesshelper.cpp \
    Style_widget/rotate_view.cpp \
    Style_widget/spacer.cpp \
    Style_widget/titlebar.cpp \
    Style_widget/tray.cpp \
    Tests/configtest.cpp \
    main.cpp \
    main_fliter.cpp \
    mainwindow.cpp \
    new_capture/Capture_button_action.cpp \
    new_capture/Widgets/Scroll_handler/Scroll_handle_global.cpp \
    new_capture/Widgets/Scroll_handler/dispatcher_worker.cpp \
    new_capture/Widgets/Scroll_handler/scroll_dispatcher.cpp \
    new_capture/Widgets/Scroll_handler/scroll_handle.cpp \
    new_capture/Widgets/Scroll_handler/scroll_worker.cpp \
    new_capture/Widgets/capture_area.cpp \
    new_capture/Widgets/capture_region.cpp \
    new_capture/Widgets/stretch_point.cpp \
    new_capture/Window_fliter.cpp \
    new_capture/capture_window.cpp

HEADERS += \
    Base/Record_element.h \
    Base/Serializable.h \
    Base/Tab_base.h \
    Base/Window_base.h \
    Helper/EnumReflect.h \
    Helper/Pool/mpool.h \
    Helper/Reflect.h \
    Helper/Serialize.h \
    Helper/common.h \
    Helper/debug.h \
    Helper/dump.h \
    Helper/image_helper.h \
    Helper/math.h \
    Helper/mstring.h \
    Helper/plist.h \
    Manager/Data/Reply_timeout.h \
    Manager/Data/update_data.h \
    Manager/Data/update_dialog.h \
    Manager/IKey_listener.h \
    Manager/config.h \
    Manager/key_manager.h \
    Manager/update.h \
    Manager/window_manager.h \
    Paint/Data/Common.h \
    Paint/Data/History_data.h \
    Paint/Data/Ilayer.h \
    Paint/Data/paint_data.h \
    Paint/Data/scroll_button.h \
    Paint/Data/stretch_button.h \
    Paint/Widgets/IControl_layer_change.h \
    Paint/Widgets/Layer_list/list_item.h \
    Paint/Widgets/Layer_list/list_widget.h \
    Paint/Widgets/Layers/LayerItems/expandbutton.h \
    Paint/Widgets/Layers/LayerItems/paintitem.h \
    Paint/Widgets/Layers/LayerItems/scrollitem.h \
    Paint/Widgets/Layers/arrowlayer.h \
    Paint/Widgets/Layers/baselayer.h \
    Paint/Widgets/Layers/blurlayer.h \
    Paint/Widgets/Layers/paint_layer.h \
    Paint/Widgets/Layers/picture_layer.h \
    Paint/Widgets/Layers/rect_layer.h \
    Paint/Widgets/Layers/shapelayer.h \
    Paint/Widgets/Layers/text_edit.h \
    Paint/Widgets/Layers/text_layer.h \
    Paint/Widgets/Panels/close_dialog.h \
    Paint/Widgets/Panels/color_selector.h \
    Paint/Widgets/Panels/flow_edit_panel.h \
    Paint/Widgets/Panels/paint_setting_panel.h \
    Paint/Widgets/Recorder_element/paint_record.h \
    Paint/Widgets/Recorder_element/paintdeleterecord.h \
    Paint/Widgets/Recorder_element/resize_record.h \
    Paint/Widgets/history.h \
    Paint/Widgets/paint_area.h \
    Paint/Widgets/recorder.h \
    Paint/Widgets/style_manager.h \
    Paint/paint_window.h \
    Panel/Widgets/bool_tab.h \
    Panel/Widgets/combo_tab.h \
    Panel/Widgets/filechooser.h \
    Panel/Widgets/key_tab.h \
    Panel/Widgets/num_tab.h \
    Panel/Widgets/tab_widget.h \
    Panel/setting.h \
    Style_widget/framelesshelper.h \
    Style_widget/rotate_view.h \
    Style_widget/spacer.h \
    Style_widget/titlebar.h \
    Style_widget/tray.h \
    Tests/AllTests.h \
    Tests/configtest.h \
    main_fliter.h \
    mainwindow.h \
    new_capture/Capture_button_action.h \
    new_capture/Widgets/Ipoint_position_change.h \
    new_capture/Widgets/Scroll_handler/Scroll_handler_global.h \
    new_capture/Widgets/Scroll_handler/dispatcher_worker.h \
    new_capture/Widgets/Scroll_handler/scroll_dispatcher.h \
    new_capture/Widgets/Scroll_handler/scroll_handle.h \
    new_capture/Widgets/Scroll_handler/scroll_worker.h \
    new_capture/Widgets/capture_area.h \
    new_capture/Widgets/capture_region.h \
    new_capture/Widgets/stretch_point.h \
    new_capture/capture_window.h \
    new_capture/window_fliter.h

FORMS += \
    Paint/paint_window.ui \
    Panel/setting.ui \
    mainwindow.ui \
    new_capture/capture_window.ui

TRANSLATIONS += \
    capture_zh_CN.ts

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

QMAKE_CXXFLAGS_RELEASE = $$QMAKE_CFLAGS_RELEASE_WITH_DEBUGINFO
QMAKE_LFLAGS_RELEASE = $$QMAKE_LFLAGS_RELEASE_WITH_DEBUGINFO


RESOURCES += \
    Resource/Resources.qrc\

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

INCLUDEPATH += F:/capture/opencv/opencv4.5.1/include \
    F:/capture/opencv/opencv4.5.1/include/opencv2

#LIBS += F:/capture/opencv/opencv4.5.1/x64/mingw/lib/libopencv_*
win32:CONFIG(release, debug|release): LIBS += -LF:/capture/opencv/opencv4.5.1-msvc/x64/vc16/lib\
-lopencv_xfeatures2d451 -lopencv_imgproc451 -lopencv_imgcodecs451\
-lopencv_core451 -lopencv_flann451 -lopencv_calib3d451 -lopencv_features2d451
else:win32:CONFIG(debug, debug|release): LIBS += -LF:/capture/opencv/opencv4.5.1-msvc/x64/vc16/lib\
-lopencv_xfeatures2d451d -lopencv_imgproc451d -lopencv_imgcodecs451d\
-lopencv_core451d -lopencv_flann451d -lopencv_calib3d451d -lopencv_features2d451d
else:unix: LIBS += -LF:/capture/opencv/opencv4.5.1-msvc/x64/vc16/lib\
-lopencv_xfeatures2d451d -lopencv_imgproc451d -lopencv_imgcodecs451d\
-lopencv_core451d -lopencv_flann451d -lopencv_calib3d451d -lopencv_features2d451d

msvc {
    QMAKE_CFLAGS += /utf-8
    QMAKE_CXXFLAGS += /utf-8
}

DEPENDPATH += C:/usr/software/Visual_Leak_Detector/include
INCLUDEPATH += C:/usr/software/Visual_Leak_Detector/include
win32:CONFIG(release, debug|release): LIBS += -LC:/usr/software/Visual_Leak_Detector/lib/Win64 -lvld
