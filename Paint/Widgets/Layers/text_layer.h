#ifndef TEXT_LAYER_H
#define TEXT_LAYER_H
#include<QWidget>
#include<QLineEdit>
#include<QTextEdit>
#include<QPlainTextEdit>
#include "Paint/Data/Ilayer.h"
#include "Helper/plist.h"
#include "Paint/Data/stretch_button.h"
#include "text_edit.h"
#include "Paint/Data/Common.h"

class Text_layer : public QWidget, public Ilayer
{
public:
    Text_layer(QRect bound_rect, QWidget* parent=nullptr);
    ~Text_layer();
    void paint(QPainter* painter, QList<QColor> disable_color, bool is_save)override;
    void set_name(QString name)override;
    void get_focus() override;
    void lose_focus() override;
    void mouse_enter(int key_code) override;
    void mouse_move(int dx, int dy) override;
    QRect bounded_rect()override;
    QString get_name()override;
    void double_click() override;
    void mouse_release() override;
    void on_size_change(int index, int dx, int dy) override;
public slots:
    void on_button_move(Stretch_button::direction dir, int dx, int dy);
//    void cursor_pos_change(int pos);
private:
    struct draw_font_info
    {
        int len;
        int height;
        QString data;
        QFont font;
        QColor color;
    };
    paint_node* root;
    paint_node* now;
    font_data now_data;
    int now_begin_pos;
    QWidget* parent;
    QString name;
    Text_edit* edit;
    QRect bound;
    QString str;
    bool has_focus;
    bool has_double_click;
    bool is_press;
    bool is_font_change;
    PList<Stretch_button*> buttons;
    void move_to_loc();
    void append_node(int len, QFont font, QColor color);
    void remove_addition_node();
//    void remove_text(int len, int begin_pos);
//    void remove_node(paint_node* node);
};

#endif // TEXT_LAYER_H
