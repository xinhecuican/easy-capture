#ifndef BUTTON_GROUP_H
#define BUTTON_GROUP_H
#include "stretch_button.h"
#include "Common.h"
#include "Ilayer.h"
#include "scroll_button.h"
#include "Helper/math.h"

class Button_group : public QWidget
{
    Q_OBJECT
public:
    Button_group(QRect rect, QWidget* parent, Ilayer* owner);
    ~Button_group();
    void set_constraints(QRect rect);
    void translate(int dx, int dy);
    void move_button(int index, int dx, int dy);
    void hide_buttons();
    void show_buttons();
    void open_scroll();
signals:
    void button_move(direction dir, int dx, int dy);
    void rotate(float delta_angle, QPoint topleft, QPoint topright, QPoint bottomleft, QPoint bottomright);
private:
    QList<Stretch_button*> buttons;
    QWidget* parent;
    QRect bound;
    Ilayer* owner;
    Line up_dir;
    float angle;
    Scroll_button* scroll_button;
    void set_buttons();
};

#endif // BUTTON_GROUP_H
