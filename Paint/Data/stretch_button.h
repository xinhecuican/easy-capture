#ifndef STRETCH_BUTTON_H
#define STRETCH_BUTTON_H
#include<QPushButton>

class Stretch_button : public QPushButton
{
    Q_OBJECT
public:
    static const int OFFSET = 5;
    enum direction{NW, NE, SE, SW};
    Stretch_button();
    ~Stretch_button();
    Stretch_button(direction dir, QWidget* parent);
    void set_constraints(QRect rect);
    void mousePressEvent(QMouseEvent* event);
    void mouseMoveEvent(QMouseEvent* event);
    void mouseReleaseEvent(QMouseEvent* event);
    void translate(int dx, int dy);
signals:
    void button_move(direction dir, int dx, int dy);
    void button_click(bool is_enter, int dx=10000, int dy=10000);
private:
    direction dir;
    QPoint point;
    bool is_click;
    QRect constraint;
    bool is_set_constraint;
    int deltax, deltay;
};

#endif // STRETCH_BUTTON_H
