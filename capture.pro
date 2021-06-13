QT       += core gui

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
    Helper/my_math.cpp \
    Manager/config.cpp \
    Manager/key_manager.cpp \
    Manager/window_manager.cpp \
    Panel/setting.cpp \
    Widgets/Capture_widgets/capture_area.cpp \
    Widgets/Capture_widgets/capture_region.cpp \
    Widgets/Capture_widgets/stretch_point.cpp \
    Widgets/Setting_widgets/bool_tab.cpp \
    Widgets/Setting_widgets/combo_tab.cpp \
    Widgets/Setting_widgets/tab_widget.cpp \
    main.cpp \
    mainwindow.cpp \
    new_capture/Capture_button_action.cpp \
    new_capture/capture_window.cpp

HEADERS += \
    Base/Tab_base.h \
    Base/Window_base.h \
    Helper/Reflect.h \
    Helper/debug.h \
    Helper/my_math.h \
    Helper/plist.h \
    Manager/config.h \
    Manager/key_manager.h \
    Manager/window_manager.h \
    Panel/setting.h \
    Widgets/Capture_widgets/capture_area.h \
    Widgets/Capture_widgets/capture_region.h \
    Widgets/Capture_widgets/stretch_point.h \
    Widgets/Setting_widgets/bool_tab.h \
    Widgets/Setting_widgets/combo_tab.h \
    Widgets/Setting_widgets/tab_widget.h \
    mainwindow.h \
    new_capture/Capture_button_action.h \
    new_capture/capture_window.h

FORMS += \
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
