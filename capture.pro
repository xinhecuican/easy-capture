QT       += core gui xml

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
    Helper/mstring.cpp \
    Helper/my_math.cpp \
    Helper/stackdumper.cpp \
    Manager/config.cpp \
    Manager/key_manager.cpp \
    Manager/window_manager.cpp \
    Paint/Widgets/Layer_list/list_item.cpp \
    Paint/Widgets/Layer_list/list_widget.cpp \
    Paint/Widgets/history.cpp \
    Paint/Widgets/paint_area.cpp \
    Paint/Widgets/paint_layer.cpp \
    Paint/Widgets/paint_setting_panel.cpp \
    Paint/Widgets/picture_layer.cpp \
    Paint/Widgets/recorder.cpp \
    Paint/Widgets/style_manager.cpp \
    Paint/paint_window.cpp \
    Panel/Widgets/combo_tab.cpp \
    Panel/Widgets/tab_widget.cpp \
    Panel/Widgets/bool_tab.cpp \
    Panel/setting.cpp \
    Style_widget/spacer.cpp \
    main.cpp \
    mainwindow.cpp \
    new_capture/Capture_button_action.cpp \
    new_capture/Widgets/capture_area.cpp \
    new_capture/Widgets/capture_region.cpp \
    new_capture/Widgets/stretch_point.cpp \
    new_capture/capture_window.cpp

HEADERS += \
    Base/Record_element.h \
    Base/Serializable.h \
    Base/Tab_base.h \
    Base/Window_base.h \
    Helper/EnumReflect.h \
    Helper/Reflect.h \
    Helper/Serialize.h \
    Helper/debug.h \
    Helper/mstring.h \
    Helper/my_math.h \
    Helper/plist.h \
    Helper/stackdumper.h \
    Manager/config.h \
    Manager/key_manager.h \
    Manager/window_manager.h \
    Paint/Data/History_data.h \
    Paint/Data/Ilayer.h \
    Paint/Data/paint_data.h \
    Paint/Widgets/IControl_layer_change.h \
    Paint/Widgets/Layer_list/list_item.h \
    Paint/Widgets/Layer_list/list_widget.h \
    Paint/Widgets/history.h \
    Paint/Widgets/paint_area.h \
    Paint/Widgets/paint_setting_panel.h \
    Paint/Widgets/picture_layer.h \
    Paint/Widgets/recorder.h \
    Paint/Widgets/style_manager.h \
    Paint/paint_window.h \
    Panel/Widgets/bool_tab.h \
    Panel/Widgets/combo_tab.h \
    Panel/Widgets/tab_widget.h \
    Panel/setting.h \
    Style_widget/spacer.h \
    mainwindow.h \
    new_capture/Capture_button_action.h \
    new_capture/Widgets/Ipoint_position_change.h \
    new_capture/Widgets/capture_area.h \
    new_capture/Widgets/capture_region.h \
    new_capture/Widgets/stretch_point.h \
    new_capture/capture_window.h

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

RESOURCES += \
    Resource/Resources.qrc\
