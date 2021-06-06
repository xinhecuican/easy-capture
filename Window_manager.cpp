#include "window_manager.h"
#include <QDebug>

Window_manager::Window_manager()
{

}

map<QString, Window_base*> Window_manager::window_list = map<QString, Window_base*>();
QString Window_manager::active_window = NULL;
QString Window_manager::previous_window = NULL;

void Window_manager::push_window(QString name, Window_base *widget)
{
    window_list[name] = widget;
    qDebug() << name;
}

void Window_manager::change_window(QString name)
{
    qDebug() << name << " " << active_window;
    if(window_list.find(name) != window_list.end() && active_window != name)
    {
        if(active_window == NULL)
        {
            active_window = name;
        }
        else if(active_window == "main")
        {
            window_list["main"]->setWindowOpacity(0);
            window_list["main"]->on_window_cancal();
        }
        else
        {
            window_list[active_window]->hide();
            window_list[active_window]->on_window_cancal();
        }


        previous_window = active_window;
        active_window = name;
        window_list[active_window]->on_window_select();
        if(name == "main")
        {
            window_list["main"]->setWindowOpacity(1);
        }
        else
        {
            window_list[name]->show();
        }
    }
}

QString Window_manager::get_now_window()
{
    return active_window;
}

void Window_manager::pop_window()
{
    change_window(previous_window);
}

bool Window_manager::contains(QString name)
{
    return window_list.find(name) != window_list.end();
}
