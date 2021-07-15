#include "capture_button_action.h"
#include "Manager/window_manager.h"
#include<QDebug>
#include<QGuiApplication>
#include<QScreen>
#include<QDesktopWidget>
#include<QApplication>
#include "Manager/config.h"
#include "hook.h"


Capture_button_action::Capture_button_action(QAbstractButton* source, QWidget* parent)
{
    this->source_button = source;
    this->parent = parent;
    connect(source_button, SIGNAL(clicked()), this, SLOT(on_button_click()));
}

void Capture_button_action::on_button_click()
{
    if(Config::get_config(Config::total_capture))
    {
        Config::set_config(Config::total_capture, false);
        Window_manager::change_window("Paint_window");
        QScreen *screen = QGuiApplication::primaryScreen();
        QPixmap map = screen->grabWindow(0);
        Window_manager::get_window("Paint_window")->
                set_pic(map, QApplication::desktop()->screenGeometry());
        return;
    }
    else
    {
        Window_manager::change_window("Capture_window");
    }
    Config::set_config(Config::rect_capture, true);
}

Capture_button_action::~Capture_button_action()
{
}
