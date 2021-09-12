#include "text_layer.h"
#include "Paint/Widgets/Recorder_element/resize_record.h"
#include "Paint/Widgets/recorder.h"
#include<QVBoxLayout>
#include<QTextOption>
#include "Paint/Widgets/Panels/flow_edit_panel.h"

Text_layer::Text_layer(QRect bound_rect, QWidget* parent):QWidget(parent)
{
    this->parent = parent;
    edit = new QPlainTextEdit(parent);
    edit->setStyleSheet("background:transparent;");
    edit->setFocusPolicy(Qt::StrongFocus);
    edit->setReadOnly(false);
    edit->setBackgroundVisible(false);
    edit->setLineWrapMode(QPlainTextEdit::WidgetWidth);
    edit->document()->setDefaultTextOption(QTextOption(Qt::AlignHCenter));
    edit->hide();
    this->bound = bound_rect;
    this->has_focus = false;
    this->has_double_click = false;
    for(int i=0; i<4; i++)
    {
        Stretch_button* button = new Stretch_button(Stretch_button::direction(i), parent);
        button->move(i == 1 || i == 2 ? bound.right()-Stretch_button::OFFSET : bound.left()-Stretch_button::OFFSET,
                     i == 2 || i == 3 ? bound.bottom()-Stretch_button::OFFSET : bound.top()-Stretch_button::OFFSET);
        connect(button, &Stretch_button::button_move, this, &Text_layer::on_button_move);
        connect(button, &Stretch_button::button_click, this, [=](bool is_enter, int dx, int dy){
            if(!is_enter)
            {
                Resize_record* record = new Resize_record(this, i, dx, dy);
                Recorder::instance()->record(record);
                parent->update();
            }
        });
        button->show();
        buttons.append(button);
    }
}

void Text_layer::paint(QPainter *painter, QList<QColor> disable_color, bool is_save)
{
    if(!has_double_click)
    {
        painter->setFont(Flow_edit_panel::instance()->get_font());
        QPen pen;
        pen.setColor(Flow_edit_panel::instance()->get_color());
        painter->setPen(pen);
        painter->drawText(bound, Qt::AlignHCenter, str);
    }
    if(!is_save && has_focus)
    {
        QPen pen;
        pen.setStyle(Qt::DotLine);
        painter->setPen(pen);
        painter->drawRect(bound);
    }
}

QRect Text_layer::bounded_rect()
{
    return bound;
}

void Text_layer::set_name(QString name)
{
    this->name = name;
}

QString Text_layer::get_name()
{
    return name;
}

void Text_layer::get_focus()
{
    has_focus = true;
    for(int i=0; i<buttons.size(); i++)
    {
        buttons[i]->show();
    }
}

void Text_layer::lose_focus()
{
    has_focus = false;
    if(has_double_click)
    {
        str = edit->toPlainText();
        edit->hide();
        parent->update();
        has_double_click = false;
    }
    for(int i=0; i<buttons.size(); i++)
    {
        buttons[i]->hide();
    }
    Flow_edit_panel::instance()->hide();
}

void Text_layer::move_to_loc()
{
    for(int i=0; i<buttons.size(); i++)
    {
        buttons[i]->move(i == 1 || i == 2 ? bound.right()-Stretch_button::OFFSET : bound.left()-Stretch_button::OFFSET,
                     i == 2 || i == 3 ? bound.bottom()-Stretch_button::OFFSET : bound.top()-Stretch_button::OFFSET);
    }
}

void Text_layer::on_button_move(Stretch_button::direction dir, int dx, int dy)
{
    QRect temp = bounded_rect();
    temp.translate(-8, -8);
    temp.setWidth(temp.width() + 16);
    temp.setHeight(temp.height() + 16);
    switch (dir)
    {
    case Stretch_button::NE:
        buttons[Stretch_button::NW]->translate(0, dy);
        buttons[Stretch_button::SE]->translate(dx, 0);
        bound.setTopRight(bound.topRight() + QPoint(dx, dy));
        break;
    case Stretch_button::NW:
        buttons[Stretch_button::NE]->translate(0, dy);
        buttons[Stretch_button::SW]->translate(dx, 0);
        bound.setTopLeft(bound.topLeft() + QPoint(dx, dy));
        break;
    case Stretch_button::SE:
        buttons[Stretch_button::NE]->translate(dx, 0);
        buttons[Stretch_button::SW]->translate(0, dy);
        bound.setBottomRight(bound.bottomRight() + QPoint(dx, dy));
        break;
    case Stretch_button::SW:
        buttons[Stretch_button::SE]->translate(0, dy);
        buttons[Stretch_button::NW]->translate(dx, 0);
        bound.setBottomLeft(bound.bottomLeft() + QPoint(dx, dy));
        break;
    }
    edit->resize(bound.width(), bound.height());
    edit->move(bound.x(), bound.y());
    parent->update(temp);
}

void Text_layer::double_click()
{
    has_double_click = true;
    QTextCharFormat format;
    format.setForeground(Flow_edit_panel::instance()->get_color());
    edit->mergeCurrentCharFormat(format);
    edit->setFont(Flow_edit_panel::instance()->get_font());
    edit->resize(bound.width(), bound.height());
    edit->move(bound.x(), bound.y());
    edit->setPlainText(str);
    edit->show();
    edit->setFocus();
    Flow_edit_panel::instance()->show();
}

void Text_layer::on_size_change(int index, int dx, int dy)
{
    this->buttons[index]->translate(dx, dy);
    on_button_move((Stretch_button::direction)index, dx, dy);
    QRect rect = bounded_rect();
    rect.setTopLeft(rect.topLeft() - QPoint(3, 3));
    rect.setBottomRight(rect.bottomRight() + QPoint(3, 3));
    parent->update(rect);
}

void Text_layer::mouse_move(int dx, int dy)
{
    if(is_press)
    {
        for(int i=0; i<buttons.size(); i++)
        {
            buttons[i]->translate(dx, dy);
        }
        edit->move(edit->x()+dx, edit->y()+dy);
        QRect temp = bound;
        bound.translate(dx, dy);
        temp = temp.united(bound);
        temp.setTopLeft(temp.topLeft() + QPoint(-10, -10));
        temp.setBottomRight(temp.bottomRight()+QPoint(10, 10));
        parent->update(temp);
    }
}

void Text_layer::mouse_enter(int key_code)
{
    if(key_code == Qt::LeftButton)
    {
        is_press = true;
    }
}

void Text_layer::mouse_release()
{
    is_press = false;
}
