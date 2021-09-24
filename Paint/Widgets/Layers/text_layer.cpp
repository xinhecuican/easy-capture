#include "text_layer.h"
#include "Paint/Widgets/Recorder_element/resize_record.h"
#include "Paint/Widgets/recorder.h"
#include<QVBoxLayout>
#include<QTextOption>
#include "Paint/Widgets/Panels/flow_edit_panel.h"

Text_layer::Text_layer(QRect bound_rect, QWidget* parent):QWidget(parent)
{
    this->parent = parent;
    is_font_change = false;
    root = new paint_node;
    root->next = NULL;
    now = NULL;
    now_data.font = Flow_edit_panel::instance()->get_font();
    now_data.color = Flow_edit_panel::instance()->get_color();

    edit = new Text_edit(parent);
    edit->setStyleSheet("background:transparent;");
    edit->setFocusPolicy(Qt::StrongFocus);
    edit->setReadOnly(false);
    edit->setBackgroundVisible(false);
    edit->setLineWrapMode(QPlainTextEdit::WidgetWidth);
    edit->document()->setDefaultTextOption(QTextOption(Qt::AlignHCenter));
    edit->hide();
//    connect(edit, &QPlainTextEdit::cursorPositionChanged, this, [=](){
//        int pos = edit->textCursor().position();
//        cursor_pos_change(pos);
//    });
//    connect(edit, &Text_edit::text_change, this, [=](int pos, int remove, int add, QString text){
//        QTextCursor cursor = edit->textCursor();
//        if(cursor.position() != pos)
//        {
//            cursor_pos_change(pos);
//        }
//        if(remove > 0)
//        {
//            remove_text(remove, pos);
//        }
//        if(add > 0)
//        {
//            if(root->next == NULL)
//            {
//                paint_node* new_node = new paint_node;
//                new_node->len = add;
//                new_node->data = now_data;
//                new_node->next = NULL;
//                now = new_node;
//                root->next = new_node;
//                is_font_change = false;
//            }
//            else
//            {

//            }
//        }
//    });
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

Text_layer::~Text_layer()
{
    for(int i=0; i<buttons.size(); i++)
    {
        delete buttons[i];
    }
    paint_node* temp = root;
    while(temp != NULL)
    {
        paint_node* temp1 = temp;
        temp = temp->next;
        delete temp1;
    }
}

void Text_layer::paint(QPainter *painter, QList<QColor> disable_color, bool is_save)
{
    if(!has_double_click)
    {
        paint_node* temp = root->next;
        int now_text_len = 0;
        int now_height = 0;
        int next_height = 0;
        int col_pos = 0;
        QList<draw_font_info> addition_data;
        QRect text_bound = bound;
        text_bound.setTopLeft(text_bound.topLeft() + QPoint(2, 2));
        text_bound.setBottomRight(text_bound.bottomRight() - QPoint(2, 2));
        if(text_bound.height() < 0 || text_bound.width() < 0)
        {
            return;
        }
        while(temp != NULL)
        {
            painter->setFont(temp->data.font);
            QPen pen;
            pen.setColor(temp->data.color);
            painter->setPen(pen);
            QFontMetrics metrics = painter->fontMetrics();
            next_height = next_height > metrics.height() ? next_height : metrics.height();
            QString temp_str = str.mid(now_text_len, temp->len);
            now_text_len += temp->len;
            temp = temp->next;
            QString insert_str;
            int now_width = 0;
            for(int i=0; i<temp_str.size(); i++)
            {
                now_width += metrics.horizontalAdvance(temp_str.at(i));
                if(now_width + col_pos > text_bound.width() || temp_str.at(i) == '\xa')
                {
                    int now_len = 0;
                    if(temp_str.at(i) == '\xa')
                    {
                        now_len = (text_bound.width() + metrics.horizontalAdvance(temp_str.at(i)) - now_width - col_pos)/2;
                        col_pos += now_len;
                    }
                    if(col_pos != 0)
                    {
                        QFont save_font = painter->font();
                        QColor save_color = painter->pen().color();
                        for(int i=0; i<addition_data.size(); i++)
                        {
                            painter->setFont(addition_data[i].font);
                            pen.setColor(addition_data[i].color);
                            painter->setPen(pen);
                            int add_len = next_height > addition_data[i].height ?
                                        (next_height - addition_data[i].height)/2 +
                                        addition_data[i].height : next_height;
                            painter->drawText(text_bound.left()+now_len,
                                              text_bound.top()+now_height+add_len, addition_data[i].data);
                            now_len += addition_data[i].len;
                        }
                        painter->setFont(save_font);
                        pen.setColor(save_color);
                        painter->setPen(pen);
                        addition_data.clear();
                    }
                    int add_len = next_height > metrics.height() ?
                                (next_height - metrics.height())/2 + metrics.height() : next_height;
                    painter->drawText(text_bound.left()+col_pos, text_bound.top()+now_height+add_len, insert_str);
                    insert_str.clear();
                    now_height += next_height;
                    next_height = metrics.height();
                    col_pos = 0;
                    if(temp_str.at(i) == '\xa')
                    {
                        now_width = 0;
                    }
                    else
                    {
                        now_width = metrics.horizontalAdvance(temp_str.at(i));
                    }
                    if(now_height + next_height > text_bound.height())
                    {
                        goto EXIT;
                    }
                }
                insert_str.append(temp_str.at(i));
            }
            if(!insert_str.isEmpty())
            {
                draw_font_info info;
                info.len = now_width;
                col_pos += now_width;
                info.data = insert_str;
                info.height = metrics.height();
                info.font = painter->font();
                info.color = painter->pen().color();
                addition_data.append(info);
            }
        }
        if(col_pos != 0)//还有剩余数据
        {
            int begin_col_pos = (text_bound.width() - col_pos)/2;
            for(int i=0; i<addition_data.size(); i++)
            {
                painter->setFont(addition_data[i].font);
                QPen pen;
                pen.setColor(addition_data[i].color);
                painter->setPen(pen);
                int add_len = next_height > addition_data[i].height ?
                            (next_height - addition_data[i].height)/2 + addition_data[i].height : next_height;
                painter->drawText(text_bound.left()+begin_col_pos,
                                  text_bound.top()+now_height+add_len, addition_data[i].data);
                begin_col_pos += addition_data[i].len;
            }
        }
EXIT:;
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
        QString text = edit->toPlainText();
        edit->moveCursor(QTextCursor::Start);
        edit->moveCursor(QTextCursor::Right);
        now = NULL;
        QTextCharFormat format = edit->textCursor().charFormat();
        int pos = 0;
        //记录所有格式
        for(int i=0; i<text.size(); i++, edit->moveCursor(QTextCursor::Right))
        {
            if(format != edit->textCursor().charFormat())
            {
                append_node(i-pos, format.font(),  format.foreground().color());
                pos = i;
                format = edit->textCursor().charFormat();
            }
        }
        if(pos != text.size())
        {
            append_node(text.size() - pos, format.font(), format.foreground().color());
        }
        remove_addition_node();
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
    edit->resize(bound.width(), bound.height());
    edit->move(bound.x(), bound.y());
//    paint_node* temp = root->next;
//    edit->moveCursor(QTextCursor::Start);
//    int pos = 0;
//    edit->clear();
//    while(temp != NULL)
//    {
//        QTextCharFormat format;
//        format.setFont(temp->data.font);
//        format.setForeground(QBrush(temp->data.color));
//        edit->textCursor().insertText(str.mid(pos, temp->len), format);
//        pos += temp->len;
//        temp = temp->next;
//    }
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

void Text_layer::append_node(int len, QFont font, QColor color)
{
    if(now == NULL)
    {
        if(root->next == NULL)
        {
            paint_node* new_node = new paint_node;
            new_node->data.font = font;
            new_node->data.color = color;
            new_node->len = len;
            new_node->next = NULL;
            root->next = new_node;
            now = new_node;
        }
        else
        {
            now = root->next;
            now->data.font = font;
            now->data.color = color;
            now->len = len;
        }
    }
    else
    {
        if(now->next == NULL)
        {
            paint_node* new_node = new paint_node;
            new_node->data.font = font;
            new_node->data.color = color;
            new_node->len = len;
            new_node->next = NULL;
            now->next = new_node;
            now = new_node;
        }
        else
        {
            now = now->next;
            now->data.font = font;
            now->data.color = color;
            now->len = len;
        }
    }
}

void Text_layer::remove_addition_node()
{
    paint_node* temp = now->next;
    while(temp != NULL)
    {
        paint_node* temp2 = temp;
        temp = temp->next;
        delete temp2;
    }
    now->next = NULL;
}

//void Text_layer::cursor_pos_change(int pos)
//{
//    int now = 0;
//    paint_node* temp = root->next;
//    while(temp != NULL)
//    {
//        now += temp->len;
//        if(now > pos)
//        {
//            this->now = temp;
//            this->now_begin_pos = now - temp->len;
//            break;
//        }
//        temp = temp->next;
//    }
//}

//void Text_layer::remove_text(int len, int begin_pos)
//{
//    QTextCursor cursor = edit->textCursor();
//    if(cursor.position() != begin_pos)
//    {
//        cursor_pos_change(begin_pos);
//    }
//    paint_node* temp_now;
//    if(now_begin_pos == begin_pos)
//    {
//        if(now == root->next)
//        {
//            temp_now = NULL;
//        }
//        else
//        {
//            temp_now = now->prev;
//            now_begin_pos -= now->prev->len;
//        }
//    }
//    else
//    {
//        temp_now = now;
//    }
//    int can_remove_len = now->len - begin_pos + now_begin_pos;
//    paint_node* temp = now;
//    paint_node* temp2 = NULL;
//    if(can_remove_len > len)
//    {
//        now->len -= len;
//        goto DESIDE_NOW;
//    }
//    else
//    {
//        temp = now->next;
//        now->len -= can_remove_len;
//        if(now_begin_pos == begin_pos)
//        {
//            remove_node(now);
//        }
//    }
//    while(len > 0 && temp != NULL)
//    {
//        if(temp->len > len)
//        {
//            len -= len;
//            temp->len -= len;
//            break;
//        }
//        else
//        {
//            len -= temp->len;
//            temp2 = temp;
//            temp = temp->next;
//            remove_node(temp2);
//        }
//    }

//DESIDE_NOW:;
//    if(temp_now == NULL)
//    {
//        paint_node* new_node = new paint_node;
//        new_node->data = now_data;
//        is_font_change = false;
//        new_node->len = 0;
//        new_node->next = root->next;
//        root->next = now;
//        if(new_node->next != NULL)
//        {
//            new_node->prev = new_node;
//        }
//        now_begin_pos = 0;
//    }
//    else
//    {
//        now = temp_now;
//    }
//}

//void Text_layer::remove_node(paint_node *node)
//{
//    if(node == root->next)
//    {
//        root->next = node->next;
//        delete node;
//    }
//    else
//    {
//        if(node->next != NULL)
//        {
//            node->next->prev = node->prev;
//        }
//        node->prev->next = node->next;
//        delete node;
//    }
//}
