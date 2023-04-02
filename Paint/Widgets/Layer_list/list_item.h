#ifndef LIST_ITEM_H
#define LIST_ITEM_H
#include<QWidget>
#include<QLineEdit>
#include<QListWidgetItem>

class List_item : public QWidget
{
    Q_OBJECT
public:
    enum button_type{UP, DOWN, CLOSE, APPEND};
    List_item();
    List_item(QWidget* parent, QString text, bool useable);
    void setName(QString name);
    QString getName();
signals:
    void text_change(QString text);
    void button_click(button_type index);
private:
    QLineEdit* name;
};

#endif // LIST_ITEM_H
