#ifndef FRAMELESS_HELPER_H
#define FRAMELESS_HELPER_H

#include <QObject>
#include <QRect>
#include <QRubberBand>
#include <QMouseEvent>
#include <QHoverEvent>

class QWidget;
class FramelessHelperPrivate;


/*****
* CursorPosCalculator
* 计算鼠标是否位于左、上、右、下、左上角、左下角、右上角、右下角
*****/
class CursorPosCalculator
{
public:
    explicit CursorPosCalculator();
    void reset();
    void recalculate(const QPoint &globalMousePos, const QRect &frameRect);

public:
    bool m_bOnEdges : true;
    bool m_bOnLeftEdge : true;
    bool m_bOnRightEdge : true;
    bool m_bOnTopEdge : true;
    bool m_bOnBottomEdge : true;
    bool m_bOnTopLeftEdge : true;
    bool m_bOnBottomLeftEdge : true;
    bool m_bOnTopRightEdge : true;
    bool m_bOnBottomRightEdge : true;

    static int m_nBorderWidth;
    static int m_nTitleHeight;
};


/*****
* WidgetData
* 更新鼠标样式、移动窗体、缩放窗体
*****/
class WidgetData : public QObject
{
    Q_OBJECT

public:
    explicit WidgetData(FramelessHelperPrivate *d, QWidget *pTopLevelWidget, QObject *parent = 0);
    ~WidgetData();
    QWidget* widget();
    // 处理鼠标事件-划过、按下、释放、移动
    void handleWidgetEvent(QEvent *event);
    // 更新橡皮筋状态
    void updateRubberBandStatus();

    void setMax(bool bMax);
    bool isMax();
private:
    // 更新鼠标样式
    void updateCursorShape(const QPoint &gMousePos);
    // 重置窗体大小
    void resizeWidget(const QPoint &gMousePos);
    // 移动窗体
    void moveWidget(const QPoint &gMousePos);
    // 处理鼠标按下
    void handleMousePressEvent(QMouseEvent *event);
    // 处理鼠标释放
    void handleMouseReleaseEvent(QMouseEvent *event);
    // 处理鼠标移动
    void handleMouseMoveEvent(QMouseEvent *event);
    //处理鼠标双击
    void handleMouseDblClickEvent(QMouseEvent *event);
    // 处理鼠标离开
    void handleLeaveEvent(QEvent *event);
    // 处理鼠标进入
    void handleHoverMoveEvent(QHoverEvent *event);

signals:
    void sigMax(bool bMax);

private:
    FramelessHelperPrivate *d;
    QRubberBand *m_pRubberBand;
    QWidget *m_pWidget;
    QPoint m_ptDragPos;
    CursorPosCalculator m_pressedMousePos;
    CursorPosCalculator m_moveMousePos;
    bool m_bLeftButtonPressed;
    bool m_bCursorShapeChanged;
    bool m_bLeftButtonTitlePressed;
    bool m_bMax;
    QRect m_rect;
    Qt::WindowFlags m_windowFlags;
};


class FramelessHelper : public QObject
{
    Q_OBJECT

public:
    explicit FramelessHelper(QObject *parent = 0);
    ~FramelessHelper();
    // 激活窗体
    void activateOn(QWidget *topLevelWidget);
    // 移除窗体
    void removeFrom(QWidget *topLevelWidget);
    // 设置窗体移动
    void setWidgetMovable(bool movable);
    // 设置窗体缩放
    void setWidgetResizable(bool resizable);
    // 设置橡皮筋移动
    void setRubberBandOnMove(bool movable);
    // 设置橡皮筋缩放
    void setRubberBandOnResize(bool resizable);
    // 设置边框的宽度
    void setBorderWidth(uint width);
    // 设置标题栏高度
    void setTitleHeight(uint height);
    bool widgetResizable();
    bool widgetMovable();
    bool rubberBandOnMove();
    bool rubberBandOnResisze();
    uint borderWidth();
    uint titleHeight();

    void setMax(QWidget *w,bool bMax);
    bool isMax(QWidget *w);
protected:
    // 事件过滤，进行移动、缩放等
    virtual bool eventFilter(QObject *obj, QEvent *event);

signals:
    void sigMax(bool bMax);

private:
    FramelessHelperPrivate *d;
};

#endif //FRAMELESS_HELPER_H
