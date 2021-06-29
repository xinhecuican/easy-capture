#ifndef TAB_WIDGET_H
#define TAB_WIDGET_H

#include <QWidget>
#include<QVector>
#include<functional>
#include<QRadioButton>
#include<QVBoxLayout>
#include<QScrollArea>
#include "bool_tab.h"
#include "combo_tab.h"
using namespace std;

class Tab_widget : public QScrollArea
{
public:
    Tab_widget();
    Tab_widget(QWidget* parent);
    Tab_widget(QString name, QWidget* parent);
    ~Tab_widget();
    void add_bool_option(QString tab_name, QString name, int index, std::function<void (bool)> const  &f);
    void add_combo_option(QString tab_name, QString text, QVector<QString> name,
                          int begin_index, int end_index, std::function<void (int)> const &f);
    void add_spacer(QString text = QString());
    int get_default_index(QString name);
    void reset();
    bool is_dirty();
    void set_dirty(bool dirty);
    int get_begin_index(QString name);
private:
    QVector<Tab_base*> widgets;
    QVector<Bool_tab*> bool_widgets;
    QVector<Combo_tab*> combo_option_widgets;
    QString widget_name;
    QVBoxLayout* layout;
    QWidget* base;
    void init();
    bool dirty;
};

#endif // TAB_WIDGET_H
