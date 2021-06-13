#ifndef CAPTURE_WINDOW_H
#define CAPTURE_WINDOW_H

#include<QVector>
#include<QKeyEvent>
#include<QMouseEvent>
#include<QPushButton>
#include "Widgets/Capture_widgets/capture_area.h"
#include "Base/Window_base.h"


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
    void keyPressEvent(QKeyEvent* event) override;
    void on_window_cancal() override;
    void on_window_select() override;

private:
    Ui::Capture_window *ui;
    Capture_area* captured;
    QRegion total_region;
    bool button_click;
};

#endif // CAPTURE_WINDOW_H
