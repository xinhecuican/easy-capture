#ifndef COMBO_TAB_H
#define COMBO_TAB_H
#include<QVector>
#include<functional>
#include<QComboBox>
#include "Base/TabBase.h"

class Combo_tab : public QComboBox, public TabBase
{
public:
    Combo_tab();
    Combo_tab(QString tab_name, QVector<QString> name, int begin_index, int end_index,
              std::function<void (int)> const &f, QWidget* parent);
    QString getName() override;
    int getBeginIndex() override;
    int getDefaultIndex() override;
    void reset() override;
private:
    QString tab_name;
    int default_index;
    int begin_index;
    int end_index;
};

#endif // COMBO_TAB_H
