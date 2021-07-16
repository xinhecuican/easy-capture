#ifndef CAPTURE_WINDOW_H
#define CAPTURE_WINDOW_H

#include<QVector>
#include<QKeyEvent>
#include<QMouseEvent>
#include<QPushButton>
#include "new_capture/Widgets/capture_area.h"
#include "Base/Window_base.h"
#include<QTimer>


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
    Capture_area* captured;
    bool button_click;
    QPainterPath free_paint_path;
    QRect active_window_bound;
    bool is_enter;
    QTimer* timer;
};

#endif // CAPTURE_WINDOW_H
