#include "window_manager.h"
#include <QDebug>
#include "Helper/debug.h"
#include "config.h"
#include<QApplication>
#include<QThread>

QMap<QString, Window_manager::Window_data> Window_manager::window_list =
        QMap<QString, Window_manager::Window_data>();
QString Window_manager::active_window = NULL;
QString Window_manager::previous_window = NULL;
bool Window_manager::is_hidden = false;

Window_manager::Window_manager(){}

//延时删除，加快窗口间的切换速度
void Window_manager::control_window_close()
{
    int time = Config::get_config(Config::clear_interval);
    qint64 current_time = QDateTime::currentDateTime().currentSecsSinceEpoch();
    QList<Window_base*> temp_list = QList<Window_base*>();
    for(auto iter=window_list.begin(); iter!=window_list.end();)
    {
        qDebug() << is_hidden << iter.key();
        if(current_time-iter.value().time >= time&&
                (is_hidden || iter.key() != active_window))
        {
            temp_list.append(iter.value().window);
            qDebug() << iter.key();
            iter = window_list.erase(iter);
        }
        else
        {
            iter++;
        }
    }
    for(int i=0; i<temp_list.size(); i++)
    {
        temp_list[i]->on_window_close();
        temp_list[i]->deleteLater();
    }
}

void Window_manager::close_window(QString name)
{
    if(window_list.find(name) != window_list.end() && name != active_window)
    {
        window_list[name].window->on_window_close();
        window_list[name].window->deleteLater();
        window_list.remove(name);
    }
    if(name == active_window)
    {
        Debug::debug_print_warning("未关闭窗口调用close_window\n位置：Window_manager::close_window");
    }
}

void Window_manager::push_window(QString name, Window_base *widget)
{
    window_list[name] = create_data(widget);
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
//        else if(active_window == "MainWindow")
//        {
//            window_list["MainWindow"].window->setWindowOpacity(0);
//            window_list["MainWindow"].window->on_window_cancal();
//        }
        else if(window_list.contains(active_window))
        {
            window_list[active_window].time = QDateTime::currentDateTime().currentSecsSinceEpoch();
            window_list[active_window].window->hide();
            window_list[active_window].window->on_window_cancal();
        }


        previous_window = active_window;
        active_window = name;
        window_list[active_window].time = QDateTime::currentDateTime().currentSecsSinceEpoch();
        window_list[active_window].window->on_window_select();
//        if(name == "MainWindow")
//        {
//            window_list["MainWindow"].window->show();
//            window_list["MainWindow"].window->setWindowOpacity(1);
//        }
//        else
//        {
            window_list[name].window->show();
//        }
    }
}

void Window_manager::open_window(QString name)
{
    create_window(name);
    previous_window = active_window;
    active_window = name;
    window_list[name].window->show();

}

QString Window_manager::get_now_window()
{
    return active_window;
}

Window_base* Window_manager::get_window(QString name)
{
    if(contains(name))
    {
        return window_list[name].window;
    }
    return NULL;
}

Window_base* Window_manager::create_and_get_window(QString name)
{
    create_window(name);
    return window_list[name].window;
}

void Window_manager::pop_window()
{
    change_window(previous_window);
}

bool Window_manager::contains(QString name)
{
    bool ans = window_list.find(name) != window_list.end();
    return ans;
}

void Window_manager::create_window(QString name)
{
    if(window_list.find(name) == window_list.end())
    {
//        if(name == "MainWindow")
//        {
//            window_list[name] = create_data(static_cast<Window_base*>(Reflect::newInstance(name)));
//        }
//        else
//        {
//            if(window_list.find("MainWindow") == window_list.end())
//            {
//                window_list["MainWindow"] = create_data
//                        (static_cast<Window_base*>(Reflect::newInstance("MainWindow")));
//            }
            window_list[name] = create_data(static_cast<Window_base*>(
                            Reflect::newInstance(name)));
//        }
    }
}

void Window_manager::close()
{
    QApplication::quit();
}

Window_manager::Window_data Window_manager::create_data(Window_base *window)
{
    Window_data data;
    data.time = QDateTime::currentDateTime().currentSecsSinceEpoch();
    data.window = window;
    return data;
}

void Window_manager::show_now()
{
//    if(active_window == "MainWindow")
//    {
//        window_list["MainWindow"].window->setWindowOpacity(1);
//    }
    create_window(active_window);
    is_hidden = false;
    window_list[active_window].window->show();
    window_list[active_window].window->setWindowFlag(Qt::WindowSystemMenuHint, true);
//    window_list["MainWindow"].window->show();
    //window_list["MainWindow"].window->hide();
}

void Window_manager::hide_now()
{
//    if(active_window == "MainWindow")
//    {
//        window_list["MainWindow"].window->setWindowOpacity(0);
//    }
    is_hidden = true;
    window_list[active_window].window->hide();
    window_list[active_window].window->setWindowFlag(Qt::WindowSystemMenuHint, false);
}
