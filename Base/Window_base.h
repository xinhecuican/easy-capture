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

    virtual void on_window_close()
    {

    }

    virtual ~Window_base(){};
    /*
     * 载入键盘输入时触发的事件
     * 注意如果要持续触发需要自己设置一个key进行检测
     * 在构造时调用
     */
    virtual void load_key_event(QString name)
    {

    }

    //下面两个函数都是Paint_window用的
    virtual void set_pic(QPixmap pix, QRect rect){};
    virtual void on_paint_panel_close(){};

    // Capture_window 录屏全局快捷键函数
    virtual void startCaptureVideo(){};
    virtual void pauseCaptureVideo(){};
    virtual void stopCaptureVideo(){};



    /// 翻译流程，
    /// 建立Data/Languages/${window_name}/语言种类(如chinese/english等).xml
    /// 该文件夹下复制chinese.xml
    /// 将里面的中文翻译成对应语言，并且language标签变成对应语言
    Window_base(QWidget* parent, QMainWindow* super_window, QString type_name):QMainWindow(parent)
    {
        super_window->setWindowTitle("简截");
        super_window->setWindowIcon(QIcon(":/image/avator.png"));
        //MString::load_from_file(":/Data/Languages/" + type_name + "/");
//        MString::load_from_file("Data/Languages/" + type_name + "/");
        //super_window->setAttribute(Qt::WA_DeleteOnClose);
    }



};
#endif // WINDOW_BASE_H
