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
#include <opencv2/core/core.hpp>
#include<QPointer>
#include <QHBoxLayout>
#include <QGraphicsView>
#include "../GraphicsScene/paintarea.h"
#include "../Helper/EnumReflect.h"
#include "../Style_widget/bubbletipswidget.h"
#include "Widgets/videocapture.h"
#include "../GraphicsScene/Widgets/defaulttoolbar.h"

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
    HWND scroll_hwnd;
    int scrollWindowIndex;
    int height_char;
    QPoint now_point;
    QPoint end_point;
    bool begin_waiting;
    //bool combine_image(QImage image);
    bool is_finish;
    QGraphicsView* view;
    PaintArea* area;
    VideoCaptureHandler* videoCapture;
    BubbleTipsWidget* tips;
    bool isVideoCapture;
    DefaultToolbar* defaultToolbar;
};

#endif // CAPTURE_WINDOW_H
