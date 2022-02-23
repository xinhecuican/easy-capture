#ifndef CAPTURE_WINDOW_H
#define CAPTURE_WINDOW_H

#include<QVector>
#include<QKeyEvent>
#include<QMouseEvent>
#include<QPushButton>
#include "new_capture/Widgets/capture_area.h"
#include "Base/Window_base.h"
#include<QTimer>
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include "new_capture/Widgets/Scroll_handler/scroll_dispatcher.h"
#include "opencv2/core/core.hpp"
#include<QPointer>

namespace Ui {
class Capture_window;
}

class Capture_window : public Window_base
{
    Q_OBJECT

public:

    Q_INVOKABLE explicit Capture_window(QWidget *parent = nullptr);
    ~Capture_window();
    void paintEvent(QPaintEvent*) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    //void keyPressEvent(QKeyEvent* event) override;
    void on_window_cancal() override;
    void on_window_select() override;
    void load_key_event(QString name) override;

private:
    Ui::Capture_window *ui;
    QPointer<Capture_area> captured;
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
    int height_char;
    QPoint cursor_point;
    QPoint now_point;
    QImage scroll_image;
    bool begin_waiting;
    //bool combine_image(QImage image);
    Scroll_dispatcher* dispatcher;
    QImage pre_image;
    bool is_finish;
    void set_scroll_info();
    bool window_valid;
};

#endif // CAPTURE_WINDOW_H
