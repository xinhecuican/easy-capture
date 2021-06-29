#ifndef WINDOW_BASE_H
#define WINDOW_BASE_H
#include<QMainWindow>
#include<QDebug>
#include "Helper/Reflect.h"
#include "Helper/mstring.h"

class Window_base : public QMainWindow
{
public:
    Window_base()
    {
    }
    virtual void on_window_select()
    {

    }
    virtual void on_window_cancal()
    {

    }
    /*
     * 载入键盘输入时触发的事件
     * 注意如果要持续触发需要自己设置一个key进行检测
     * 在构造时调用
     */
    virtual void load_key_event(QString name)
    {

    }



    Window_base(QWidget* parent, QMainWindow* super_window, QString type_name):QMainWindow(parent)
    {
        super_window->setWindowTitle("简截");
        super_window->setWindowIcon(QIcon(":/image/avator.png"));
        MString::load_from_file(":/Data/Languages/" + type_name + "/");
    }



};
#endif // WINDOW_BASE_H
