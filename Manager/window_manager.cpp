#include "window_manager.h"
#include <QDebug>
#include "Helper/debug.h"

Window_manager::Window_manager()
{

}

map<QString, Window_base*> Window_manager::window_list = map<QString, Window_base*>();
QString Window_manager::active_window = NULL;
QString Window_manager::previous_window = NULL;

void Window_manager::push_window(QString name, Window_base *widget)
{
    window_list[name] = widget;
}

void Window_manager::change_window(QString name)
{
    qDebug() << name << " " << active_window;
    /*if(window_list.find(name) == window_list.end())
    {
        window_list[name] = static_cast<Window_base*>(Reflect::newInstance(name));
    }*/
    create_window(name);
    if(active_window != name)
    {
        if(active_window == NULL)
        {
            active_window = name;
        }
        else if(active_window == "MainWindow")
        {
            window_list["MainWindow"]->setWindowOpacity(0);
            window_list["MainWindow"]->on_window_cancal();
        }
        else
        {
            window_list[active_window]->hide();
            window_list[active_window]->on_window_cancal();
        }


        previous_window = active_window;
        active_window = name;
        window_list[active_window]->on_window_select();
        if(name == "MainWindow")
        {
            window_list["MainWindow"]->show();
            window_list["MainWindow"]->setWindowOpacity(1);
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

Window_base* Window_manager::get_window(QString name)
{
    /*if(window_list.find(name) == window_list.end())
    {
        window_list[name] = static_cast<Window_base*>(Reflect::newInstance(name));
    }*/
    create_window(name);
    return window_list[name];
}

void Window_manager::pop_window()
{
    change_window(previous_window);
}

bool Window_manager::contains(QString name)
{
    return window_list.find(name) != window_list.end();
}

void Window_manager::create_window(QString name)
{
    if(window_list.find(name) == window_list.end())
    {
        if(name == "MainWindow")
        {
            window_list[name] = static_cast<Window_base*>(Reflect::newInstance(name));
        }
        else
        {
            if(window_list.find("MainWindow") == window_list.end())
            {
                window_list["MainWindow"] = static_cast<Window_base*>(Reflect::newInstance("MainWindow"));
            }
            window_list[name] = static_cast<Window_base*>(
                            Reflect::newInstance(name, Q_ARG(QWidget*, window_list["MainWindow"])));
        }
    }
}
