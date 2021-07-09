#ifndef LIST_WIDGET_H
#define LIST_WIDGET_H
#include<QListWidget>
#include "list_item.h"

class List_widget : public QListWidget
{
    Q_OBJECT
public:
    List_widget();
    List_widget(QWidget* parent=nullptr);
    void add_widget(QString name);
    //void wheelEvent(QWheelEvent* event)override;
signals:
    void text_change(int index, QString text);
    void button_click(int index, List_item::button_type button_type);
private:
    void remove_item(int row);
    void swap_item(int before, int after);
};

#endif // LIST_WIDGET_H
