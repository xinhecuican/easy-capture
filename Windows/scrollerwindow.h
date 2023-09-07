#ifndef ScrollerWindow_H
#define ScrollerWindow_H
#include "Base/WindowBase.h"
#include "Style_widget/bubbletipswidget.h"
#include "new_capture/Widgets/Scroll_handler/scroll_dispatcher.h"
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
private:
    enum ScrollState {IDLE, SCROLLRECT_SETTING, SCROLLRECT_SETTED, SCROLL_AUTO, SCROLL_MANUAL};
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
    QImage preImage;
    qint64 lastCaptureTime;
};
#endif // ScrollerWindow_H