#ifndef WINDOW_BASE_H
#define WINDOW_BASE_H
#include<QMainWindow>
#include<QDebug>
#include "../Helper/Reflect.h"
#include "../Helper/mstring.h"
#include "../Manager/uimanager.h"
#include "../Manager/KeyManager.h"

class WindowBase : public QMainWindow {
public:
    WindowBase() {
    }

    /**
     * @brief afterInitialize 在构造函数之后调用
     * @param WindowName 窗体类名
     */
    virtual void afterInitialize(QString WindowName) {
        windowClassName = WindowName;
        loadKeyEvent(WindowName);
        try {
            setStyleSheet(UIManager::instance()->load(WindowName));
        } catch(std::exception e) {
            qCritical() << e.what();
        }
    }

    /**
     * @brief receiveData 用于窗口切换时接收参数
     * @param data1
     * @param data2
     */
    virtual void receiveData(QVariant data1=QVariant(), QVariant data2=QVariant()){
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

    /**
     * @brief controlWindowClose 是否自己控制窗体的资源释放
     * @return
     */
    virtual bool controlWindowClose(){
        return false;
    }

    /**
     * @brief disableWindowClose 如果当前窗口为活跃窗口则禁用窗口资源释放
     * @return
     */
    virtual bool disableWindowClose(){
        return false;
    }

    virtual ~WindowBase() {};

    /**
     * @brief loadKeyEvent 载入快捷键
     * @note 载入键盘输入时触发的事件
     * 注意如果要持续触发需要自己设置一个key进行检测
     * 在构造时调用
     * @param name 窗体名称
     */
    virtual void loadKeyEvent(QString name) {

    }

    virtual void keyPressEvent(QKeyEvent* event) override {
        KeyManager::instance()->keyEnter(windowClassName, event);
        return QMainWindow::keyPressEvent(event);
    }

    virtual void keyReleaseEvent(QKeyEvent* event) override {
        KeyManager::instance()->keyRelease(windowClassName, event);
        return QMainWindow::keyReleaseEvent(event);
    }

    //下面函数是Paint_window用的
    virtual void onPaintWindowClose() {};

    /// CaptureWindow 录屏全局快捷键函数
    virtual void startCaptureVideo() {};
    virtual void pauseCaptureVideo() {};
    virtual void stopCaptureVideo() {};



    /// 翻译流程，
    /// 建立Data/Languages/${window_name}/语言种类(如chinese/english等).xml
    /// 该文件夹下复制chinese.xml
    /// 将里面的中文翻译成对应语言，并且language标签变成对应语言
    WindowBase(QWidget* parent):QMainWindow(parent) {
        setWindowTitle("简截");
        setWindowIcon(QIcon(":/image/avator.png"));
    }

private:
    QString windowClassName;



};
#endif // WINDOW_BASE_H
