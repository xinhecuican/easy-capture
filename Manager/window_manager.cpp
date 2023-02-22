#include "window_manager.h"
#include <QDebug>
#include "Helper/debug.h"
#include "config.h"
#include<QApplication>
#include<QThread>
#include "key_manager.h"
#include "main_fliter.h"

QMap<QString, Window_manager::Window_data> Window_manager::window_list =
        QMap<QString, Window_manager::Window_data>();
QString Window_manager::active_window = "tray";
QString Window_manager::previous_window = NULL;
bool Window_manager::current_hidden = false;
bool Window_manager::previous_hidden = false;

Window_manager::Window_manager(){}

//延时删除，加快窗口间的切换速度
void Window_manager::control_window_close()
{
    int time = Config::getConfig<int>(Config::clear_interval);
    qint64 current_time = QDateTime::currentDateTime().currentSecsSinceEpoch();
    QList<Window_base*> temp_list = QList<Window_base*>();
    if(active_window == "Setting")return;
    qInfo() << active_window << endl;
    for(auto iter=window_list.begin(); iter!=window_list.end();)
    {
        if(current_time-iter.value().time >= time&&
                (current_hidden || iter.key() != active_window))
        {
            temp_list.append(iter.value().window);
            qInfo() << "delete " << iter.key();
            Key_manager::onWindowClose(iter.key());
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
    if(window_list.size() == 0)
    {
        Main_fliter::instance()->stop_timer();
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
        qWarning("未关闭窗口调用close_window\n位置：Window_manager::close_window");
    }
}

void Window_manager::push_window(QString name, Window_base *widget)
{
    window_list[name] = create_data(widget);
}

void Window_manager::change_window(QString name)
{
    qInfo() << name << " " << active_window;
    create_window(name);
    if(active_window != name)
    {
        Key_manager::onWindowChangeBegin(active_window, name);
        if(name == "tray")
        {
            window_list[active_window].window->setWindowFlag(Qt::WindowSystemMenuHint, false);
        }
        else
        {
            window_list[name].window->setWindowFlag(Qt::WindowSystemMenuHint, true);
        }
        if(active_window != "tray" && window_list.contains(active_window))
        {
            window_list[active_window].time = QDateTime::currentDateTime().currentSecsSinceEpoch();
            window_list[active_window].window->hide();
            window_list[active_window].window->on_window_cancal();
        }
        previous_window = active_window;
        active_window = name;
        previous_hidden = current_hidden;
        current_hidden = false;
        if(active_window != "tray")
        {
            window_list[active_window].time = QDateTime::currentDateTime().currentSecsSinceEpoch();
            window_list[active_window].window->on_window_select();
            window_list[name].window->show();
        }
        Key_manager::onWindowChangeEnd();
    }

    if(!Main_fliter::instance()->is_timer_run())
    {
        Main_fliter::instance()->start_timer();
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

void Window_manager::pop_window()
{
    if(previous_hidden)
    {
        previous_hidden = current_hidden;
        hide_now();
        QString temp_window = active_window;
        active_window = previous_window;
        previous_window = temp_window;
        return;
    }
    change_window(previous_window);
}

bool Window_manager::contains(QString name)
{
    bool ans = window_list.find(name) != window_list.end();
    return ans;
}

void Window_manager::create_window(QString name)
{
    if(name == "tray")
        return;
    if(window_list.find(name) == window_list.end())
    {
        window_list[name] = create_data(static_cast<Window_base*>(
                            Reflect::newInstance(name)));
    }
}

void Window_manager::close()
{
    for(auto iter=window_list.begin(); iter!=window_list.end(); iter++)
    {
        iter->window->on_window_close();
        iter->window->deleteLater();
    }
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
    create_window(active_window);
    current_hidden = false;
    window_list[active_window].window->show();
    window_list[active_window].window->setWindowFlag(Qt::WindowSystemMenuHint, true);
    if(!Main_fliter::instance()->is_timer_run())
    {
        Main_fliter::instance()->start_timer();
    }
}

void Window_manager::hide_now()
{
    current_hidden = true;
    window_list[active_window].window->hide();
    window_list[active_window].window->setWindowFlag(Qt::WindowSystemMenuHint, false);
}

void Window_manager::hideToMain()
{
    current_hidden = true;
    window_list[active_window].window->hide();
    window_list[active_window].window->setWindowFlag(Qt::WindowSystemMenuHint, false);
    previous_window = active_window;
    active_window = "MainWindow";
}
