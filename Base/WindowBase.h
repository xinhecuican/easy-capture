#ifndef WINDOW_BASE_H
#define WINDOW_BASE_H
#include<QMainWindow>
#include<QDebug>
#include "Helper/Reflect.h"
#include "Helper/mstring.h"

class WindowBase : public QMainWindow {
public:
    WindowBase() {
    }

    /**
     * @brief onWindowSelect 当窗体切换到前台后调用
     */
    virtual void onWindowSelect() {

    }

    /**
     * @brief onWindowCancel 当窗体切换出前台时调用
     */
    virtual void onWindowCancel() {

    }

    /**
     * @brief onWindowClose 窗体释放资源前调用
     */
    virtual void onWindowClose() {

    }

    virtual ~WindowBase() {};
    /*

     */
    /**
     * @brief loadKeyEvent 载入快捷键
     * @note 载入键盘输入时触发的事件
     * 注意如果要持续触发需要自己设置一个key进行检测
     * 在构造时调用
     * @param name 窗体名称
     */
    virtual void loadKeyEvent(QString name) {

    }

    //下面两个函数都是Paint_window用的
    /**
     * @brief setPic 设置绘图窗口的图片
     * @param pix
     * @param rect 图片的长和宽，顶点位于(0, 0)
     */
    virtual void setPic(QPixmap pix, QRect rect) {};
    virtual void onPaintWindowClose() {};

    // CaptureWindow 录屏全局快捷键函数
    virtual void startCaptureVideo() {};
    virtual void pauseCaptureVideo() {};
    virtual void stopCaptureVideo() {};



    /// 翻译流程，
    /// 建立Data/Languages/${window_name}/语言种类(如chinese/english等).xml
    /// 该文件夹下复制chinese.xml
    /// 将里面的中文翻译成对应语言，并且language标签变成对应语言
    WindowBase(QWidget* parent, QMainWindow* superWindow, QString typeName):QMainWindow(parent) {
        superWindow->setWindowTitle("简截");
        superWindow->setWindowIcon(QIcon(":/image/avator.png"));
        //MString::load_from_file(":/Data/Languages/" + type_name + "/");
//        MString::load_from_file("Data/Languages/" + type_name + "/");
        //super_window->setAttribute(Qt::WA_DeleteOnClose);
    }



};
#endif // WINDOW_BASE_H
