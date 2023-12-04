#ifndef SPACER_H
#define SPACER_H
#include<QWidget>
#include<QPushButton>
#include<QBoxLayout>
#include<QToolButton>
#include "../Helper/plist.h"

class Spacer : public QWidget {
    Q_OBJECT
public:
    enum button_state {HIDE, SHOW};
    Spacer();
    Spacer(QString name, bool need_hide_button, QWidget* parent=nullptr);
    void paintEvent(QPaintEvent *event) override;
    void set_button_state(button_state state);
    void add_widget(QWidget* widget);
    void add_layout(QLayout* layout);
private:
    QWidget* parent;
    QToolButton* hide_button;
    QString name;
    QVBoxLayout* layout;
    button_state state;
    QIcon show_icon;
    QIcon hide_icon;
    PList<QWidget*> widgets;
    void widgets_hide();
    void widgets_show();
};

#endif // SPACER_H
