#ifndef SCROLL_BUTTON_H
#define SCROLL_BUTTON_H
#include<QPushButton>

class Scroll_button : public QPushButton
{
    Q_OBJECT
public:
    Scroll_button(QWidget* parent);
    void mouseMoveEvent(QMouseEvent* event)override;
    void mousePressEvent(QMouseEvent* event)override;
    void mouseReleaseEvent(QMouseEvent* event)override;
signals:
    void button_click(QPoint pos);
    void button_move(QPoint pos);
    void button_release(QPoint pos);
};

#endif // SCROLL_BUTTON_H
