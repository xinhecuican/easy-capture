#ifndef COMBO_TAB_H
#define COMBO_TAB_H
#include<QVector>
#include<functional>
#include<QComboBox>
#include "Base/Tab_base.h"

class Combo_tab : public QComboBox, Tab_base
{
public:
    Combo_tab();
    Combo_tab(QString tab_name, QVector<QString> name, int begin_index, int end_index,
              std::function<void (int)> const &f, QWidget* parent);
    QString get_name() override;
    int get_begin_index() override;
    int get_default_index() override;
    void reset() override;
private:
    QString tab_name;
    QString text;
    int default_index;
    int begin_index;
    int end_index;
};

#endif // COMBO_TAB_H
