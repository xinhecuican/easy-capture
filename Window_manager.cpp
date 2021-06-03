#include "window_manager.h"
#include <QDebug>

Window_manager::Window_manager()
{

}

map<QString, QWidget*> Window_manager::window_list = map<QString, QWidget*>();
QString Window_manager::active_window = NULL;
QString Window_manager::previous_window = NULL;

void Window_manager::push_window(QString name, QWidget *widget)
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
        }
        else
        {
            window_list[active_window]->hide();
        }


        previous_window = active_window;
        active_window = name;
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
