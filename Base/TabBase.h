#ifndef TAB_BASE_H
#define TAB_BASE_H
#include<QString>
/**
 * @brief The TabBase class 设置窗口的菜单项基类
 */
class TabBase {
public:
    virtual QString getName()=0;

    /**
     * @brief @ref WindowBase::onWindowCancel() 时调用\n
     * 没保存就退出时会重置未保存的选项
     */
    virtual void reset()=0;

    /**
     * @brief 保存当前选项
     */
    virtual void onSave()=0;

    virtual bool getChange(){return isChange;}

    /**
     * @brief restore
     * @deprecated
     */
    virtual void restore() {}
    virtual ~TabBase() {};
protected:
    bool isChange=false;
};

#endif // TAB_BASE_H
