#ifndef WINDOW_BASE_H
#define WINDOW_BASE_H
#include<QMainWindow>

class Window_base : public QMainWindow
{
public:
    Window_base()
    {

    }

    Window_base(QWidget* parent):QMainWindow(parent)
    {

    }

    virtual void on_window_select()
    {

    }
    virtual void on_window_cancal()
    {

    }

};
#endif // WINDOW_BASE_H
