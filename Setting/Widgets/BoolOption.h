#ifndef BOOL_TAB_H
#define BOOL_TAB_H
#include<QWidget>
#include<QString>
#include<QRadioButton>
#include<functional>
#include "Base/TabBase.h"

class BoolOption : public QRadioButton, public TabBase {
    Q_OBJECT
public:
    BoolOption();
    BoolOption(QString tabName, int index, QWidget* parent);
    BoolOption(QString tab_name, int index,
               std::function<void (bool)> const  &f, QWidget* parent);
    QString getName() override;
    void reset() override;
    void onSave() override;
private:
    QString tab_name;
    bool currentOption;
    int index;
    std::function<void (bool)> saveFunc;
};

#endif // BOOL_TAB_H
