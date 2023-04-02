#ifndef BASELAYER_H
#define BASELAYER_H
#include<QGraphicsObject>

class BaseLayer {
public:
    virtual void getFocusFunc() {}
    virtual void loseFocusFunc() {}

    /**
     * @brief hideNormal 当从指针或绘制形状切换成其他模式时调用，为了不在其他模式中显示边框
     */
    Q_INVOKABLE virtual void hideNormal() {}
    Q_INVOKABLE virtual void showNormal() {}

    /**
     * @brief acceptFocus 是否被 @ref ShapeLayer::childContains() 考虑在内
     * @return
     */
    Q_INVOKABLE virtual bool acceptFocus() {
        return true;
    }
};

#endif // BASELAYER_H
