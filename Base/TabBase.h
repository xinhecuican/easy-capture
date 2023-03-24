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
     * @brief getBeginIndex 获得当前设置的序号 @ref Config
     * @return
     */
    virtual int getBeginIndex()=0;

    /**
     * @brief getDefaultIndex 多选设置修改前的序号
     * @return
     */
    virtual int getDefaultIndex()=0;
    virtual void reset()=0;
    virtual void restore() {}
    virtual ~TabBase() {};
};

#endif // TAB_BASE_H
