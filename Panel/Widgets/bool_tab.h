#ifndef BOOL_TAB_H
#define BOOL_TAB_H
#include<QWidget>
#include<QString>
#include<QRadioButton>
#include<functional>
#include "Base/TabBase.h"

class Bool_tab : public QRadioButton, public TabBase
{
public:
    Bool_tab();
    Bool_tab(QString tab_name, int index,
             std::function<void (bool)> const  &f, QWidget* parent);
    QString getName() override;
    int getBeginIndex() override;
    int getDefaultIndex() override;
    void reset() override;
private:
    QString tab_name;
    bool default_option;
    int index;
};

#endif // BOOL_TAB_H
