#ifndef BUTTON_ACTION_H
#define BUTTON_ACTION_H
#include<QPushButton>
#include<QObject>
class button_action : QWidget
{
    Q_OBJECT
public:
    void on_keybind_enter();
    explicit button_action(QPushButton* source, QWidget* parent);
    ~button_action(){};
public slots:
    void on_button_click();
private:
    QPushButton* source_button;
    QWidget* parent;
};

#endif // BUTTON_ACTION_H
