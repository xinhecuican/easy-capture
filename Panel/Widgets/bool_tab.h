#ifndef BOOL_TAB_H
#define BOOL_TAB_H
#include<QWidget>
#include<QString>
#include<QRadioButton>
#include<functional>
#include "Base/Tab_base.h"

class Bool_tab : public QRadioButton, public Tab_base
{
public:
    Bool_tab();
    Bool_tab(QString tab_name, QString name, int index,
             std::function<void (bool)> const  &f, QWidget* parent);
    QString get_name() override;
    int get_begin_index() override;
    int get_default_index() override;
    void reset() override;
private:
    QString tab_name;
    bool default_option;
    int index;
};

#endif // BOOL_TAB_H
