#ifndef CAPTURE_BUTTON_ACTION_H
#define CAPTURE_BUTTON_ACTION_H
#include<QWidget>
#include<QAbstractButton>
#include "capture_window.h"
#include<QPalette>
#include<QTimer>

class Capture_button_action : QWidget
{
    Q_OBJECT
public:
    Capture_button_action(QAbstractButton* source, QWidget* parent);
    ~Capture_button_action();
public slots:
    void on_button_click();
private:
    QAbstractButton* source_button;
    QWidget* parent;
    QPixmap whole_screen;
    bool is_capture;
    QRect qrect;
};

#endif // CAPTURE_BUTTON_ACTION_H
