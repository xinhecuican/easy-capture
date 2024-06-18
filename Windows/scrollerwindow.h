#ifndef ScrollerWindow_H
#define ScrollerWindow_H
#include "../Base/WindowBase.h"
#include "../Style_widget/bubbletipswidget.h"
#include "ScrollHandler/scroll_dispatcher.h"
#include "../Style_widget/fadelabel.h"

class ScrollerWindow : public WindowBase
{
    Q_OBJECT

public:
    Q_INVOKABLE explicit ScrollerWindow(QWidget *parent = nullptr);
    void paintEvent(QPaintEvent*) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void onWindowCancel() override;
    void onWindowSelect() override;
    void loadKeyEvent(QString name) override;
    void wheelEvent(QWheelEvent* event)override;
private:
    enum ScrollState {IDLE, SCROLLRECT_SETTING, SCROLLRECT_SETTED, SCROLL_AUTO, SCROLL_MANUAL, SCROLL_END};
    QRect screenGeometry;
    BubbleTipsWidget* bubbleTipsWidget;
    bool isScrollRect;
    bool isScrollRectEnter;
    bool isScrollRectEnd;
    bool isScrollManual;
    ScrollState scrollState;
    ScrollState beforeState;
    Scroll_dispatcher* dispatcher;
    QTimer* timer;
    QTimer* scrollTimer;
    QRect scrollBound;
    QRect activeWindowBound;
    bool end_scroll;
    HWND scroll_hwnd;
    int ScrollerWindowIndex;
    QPoint beginPoint;
    QPoint endPoint;
    QPoint currentPoint;
    QVector<QImage> preImage;
    bool scrollEndDetectBegin;
    int scrollTimes;
    int scrollEndTimes;
    qint64 lastCaptureTime;
    FadeLabel* intervalLabel;
    int detectInterval = 30;
    qint64 lastMoveTime = 0;
};
#endif // ScrollerWindow_H
