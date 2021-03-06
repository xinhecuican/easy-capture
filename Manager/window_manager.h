#ifndef WINDOW_MANAGER_H
#define WINDOW_MANAGER_H
#include<map>
#include<iostream>
#include<QWidget>
#include "Base/Window_base.h"

#include<QMutex>
#include<QTime>
#include<QDateTime>
using namespace std;

class Window_manager
{
public:
    friend class Main_fliter;
    Window_manager();
    static void push_window(QString name, Window_base* widget);
    static void change_window(QString name);
    static void open_window(QString name);
    static QString get_now_window();
    static void pop_window();
    static bool contains(QString name);
    static Window_base* get_window(QString name);
    static void close();
    static void control_window_close();
    static void close_window(QString name);
    static void show_now();
    static void hide_now();
    static void hideToMain();
private:
    struct Window_data
    {
        qint64 time;
        Window_base* window;
    };
    static QMap<QString, Window_data> window_list;
    static QString active_window;
    static QString previous_window;
    static void create_window(QString name);
    static Window_data create_data(Window_base* window);
    static bool current_hidden;
    static bool previous_hidden;
};



#endif // WINDOW_MANAGER_H
