#ifndef CAPTURE_WINDOW_H
#define CAPTURE_WINDOW_H

#include <QMainWindow>
#include<QVector>
#include<QKeyEvent>
#include<QMouseEvent>
#include<QPushButton>
#include "Widgets/capture_area.h"

namespace Ui {
class Capture_window;
}

class Capture_window : public QMainWindow
{
    Q_OBJECT

public:
    explicit Capture_window(QWidget *parent = nullptr);
    ~Capture_window();
    void paintEvent(QPaintEvent*);
    void mousePressEvent(QMouseEvent* event);
    void mouseMoveEvent(QMouseEvent* event);
    void mouseReleaseEvent(QMouseEvent* event);
    void keyPressEvent(QKeyEvent* event);

private:
    Ui::Capture_window *ui;
    QVector<QPoint> points;
    Capture_area* captured;
};

#endif // CAPTURE_WINDOW_H
