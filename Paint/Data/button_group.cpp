#include "button_group.h"
#include "Paint/Widgets/Recorder_element/resize_record.h"
#include "Paint/Widgets/recorder.h"

Button_group::Button_group(QRect rect, QWidget* parent, Ilayer* owner) : QWidget(parent)
{
    this->bound = rect;
    this->parent = parent;
    this->owner = owner;
    if(abs(bound.top() - bound.bottom()) >= 10 && abs(bound.left() - bound.right()) >= 10)
    {
        set_buttons();
    }
    up_dir = Line(QPoint(0, 0), QPoint(0, 1));
    scroll_button = NULL;
}

Button_group::~Button_group()
{
    for(int i=0; i<buttons.size(); i++)
    {
        buttons[i]->deleteLater();
    }
    if(scroll_button != NULL)
    {
        scroll_button->deleteLater();
    }
}

void Button_group::set_constraints(QRect rect)
{
    this->bound = rect;
    for(int i=0; i<buttons.size(); i++)
    {
        buttons[i]->set_constraint(rect);
    }
}

void Button_group::set_buttons()
{
    for(int i=0; i<4; i++)
    {
        Stretch_button* button = new Stretch_button(direction(i), parent);
        button->move(i == 1 || i == 2 ? bound.right()-Stretch_button::OFFSET : bound.left()-Stretch_button::OFFSET,
                     i == 2 || i == 3 ? bound.bottom()-Stretch_button::OFFSET : bound.top()-Stretch_button::OFFSET);
        connect(button, &Stretch_button::button_move, this, [=](direction dir, int dx, int dy){
            switch (dir)
            {
            case NE:
                buttons[NW]->translate(0, dy);
                buttons[SE]->translate(dx, 0);
                break;
            case NW:
                buttons[NE]->translate(0, dy);
                buttons[SW]->translate(dx, 0);
                break;
            case SE:
                buttons[NE]->translate(dx, 0);
                buttons[SW]->translate(0, dy);
                break;
            case SW:
                buttons[SE]->translate(0, dy);
                buttons[NW]->translate(dx, 0);
                break;
            }
            emit button_move(dir, dx, dy);
        });
        connect(button, &Stretch_button::button_click, this, [=](bool is_enter, int dx, int dy)
        {
            if(!is_enter)
            {
                Resize_record* record = new Resize_record(owner, i, dx, dy);
                Recorder::instance()->record(record);
                parent->update();
            }
        });
        button->show();
        buttons.append(button);
    }
}

void Button_group::hide_buttons()
{
    for(int i=0; i<buttons.size(); i++)
    {
        buttons[i]->hide();
    }
}

void Button_group::show_buttons()
{
    for(int i=0; i<buttons.size(); i++)
    {
        buttons[i]->show();
    }
}

void Button_group::move_button(int index, int dx, int dy)
{
    this->buttons[index]->translate(dx, dy);
    switch (index)
    {
    case NE:
        buttons[NW]->translate(0, dy);
        buttons[SE]->translate(dx, 0);
        break;
    case NW:
        buttons[NE]->translate(0, dy);
        buttons[SW]->translate(dx, 0);
        break;
    case SE:
        buttons[NE]->translate(dx, 0);
        buttons[SW]->translate(0, dy);
        break;
    case SW:
        buttons[SE]->translate(0, dy);
        buttons[NW]->translate(dx, 0);
        break;
    }
}

void Button_group::translate(int dx, int dy)
{
    for(int i=0; i<buttons.size(); i++)
    {
        buttons[i]->translate(dx, dy);
    }
}

void Button_group::open_scroll()
{
    scroll_button = new Scroll_button(parent);
    scroll_button->setIcon(QIcon(":/image/rotate.png"));
    scroll_button->setCursor(QCursor(QPixmap(":/image/rotate.png")));
    connect(scroll_button, &Scroll_button::button_move, this, [=](QPoint pos){
        Line now_line = Line(bound.center(), pos);
        float angle = up_dir.angle(now_line);
        this->angle += angle;
        MRect rect = Math::scroll(this->angle, bound);
        buttons[NE]->move(rect.topright);
        buttons[NW]->move(rect.topleft);
        buttons[SE]->move(rect.bottomright);
        buttons[SW]->move(rect.bottomleft);
        emit rotate(angle, rect.topleft, rect.topright, rect.bottomleft, rect.bottomright);
    });
    angle = 0;
}
