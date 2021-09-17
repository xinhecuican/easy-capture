#ifndef TEXT_LAYER_H
#define TEXT_LAYER_H
#include<QWidget>
#include<QLineEdit>
#include<QTextEdit>
#include<QPlainTextEdit>
#include "Paint/Data/Ilayer.h"
#include "Helper/plist.h"
#include "Paint/Data/stretch_button.h"

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
private:
    QWidget* parent;
    QString name;
    QPlainTextEdit* edit;
    QRect bound;
    QString str;
    bool has_focus;
    bool has_double_click;
    bool is_press;
    PList<Stretch_button*> buttons;
    void move_to_loc();
};

#endif // TEXT_LAYER_H
