#ifndef CAPTURE_WINDOW_H
#define CAPTURE_WINDOW_H

#include<QVector>
#include<QKeyEvent>
#include<QMouseEvent>
#include<QPushButton>
#include "Base/WindowBase.h"
#include<QTimer>
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include "new_capture/Widgets/Scroll_handler/scroll_dispatcher.h"
#include "opencv2/core/core.hpp"
#include<QPointer>
#include <QHBoxLayout>
#include <QGraphicsView>
#include "Paint/Widgets/PaintArea.h"
#include "Helper/EnumReflect.h"
#include "Style_widget/bubbletipswidget.h"
#include "Widgets/VideoHandler/videocapture.h"

//namespace Ui {
//class CaptureWindow;
//}

class CaptureWindow : public WindowBase {
    Q_OBJECT

public:
    Q_INVOKABLE explicit CaptureWindow(QWidget *parent = nullptr);
    ~CaptureWindow();
    void paintEvent(QPaintEvent*) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    //void keyPressEvent(QKeyEvent* event) override;
    void onWindowCancel() override;
    void onWindowSelect() override;
    void loadKeyEvent(QString name) override;
    void startCaptureVideo() override;
    void pauseCaptureVideo() override;
    void stopCaptureVideo() override;

private:
    enum ScrollState {IDLE, SCROLLRECT_SETTING, SCROLLRECT_SETTED, SCROLL_AUTO, SCROLL_MANUAL};
//    Ui::CaptureWindow *ui;
    QWidget* centralWidget;
    bool button_click;
    QPainterPath free_paint_path;
    QRect active_window_bound;
    QRect scroll_bound;
    bool is_enter;
    static bool end_scroll;
    bool is_first_capture;//第一次捕获时可能是自由捕获
    int mouse_move_times;
    QTimer* timer;
    QTimer* scroll_timer;
    HWND scroll_hwnd;
    int scrollWindowIndex;
    int height_char;
    QPoint cursor_point;
    QPoint now_point;
    QPoint end_point;
    QImage scroll_image;
    bool begin_waiting;
    //bool combine_image(QImage image);
    Scroll_dispatcher* dispatcher;
    QImage pre_image;
    bool is_finish;
    void set_scroll_info();
    bool window_valid;
    QGraphicsView* view;
    PaintArea* area;
    bool isScrollRect;
    bool isScrollRectEnter;
    bool isScrollRectEnd;
    bool isScrollManual;
    BubbleTipsWidget* bubbleTipsWidget;
    ScrollState scrollState;
    ScrollState beforeState;
    VideoCaptureHandler* videoCapture;
    qint64 lastCaptureTime;
    bool isVideoCapture;
};

#endif // CAPTURE_WINDOW_H
