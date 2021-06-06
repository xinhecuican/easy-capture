#ifndef WINDOW_MANAGER_H
#define WINDOW_MANAGER_H
#include<map>
#include<iostream>
#include<QWidget>
#include "Base/Window_base.h"
using namespace std;

class Window_manager
{
public:
    Window_manager();
    static void push_window(QString name, Window_base* widget);
    static void change_window(QString name);
    static QString get_now_window();
    static void pop_window();
    static bool contains(QString name);
private:
    static map<QString, Window_base*> window_list;
    static QString active_window;
    static QString previous_window;
};



#endif // WINDOW_MANAGER_H
