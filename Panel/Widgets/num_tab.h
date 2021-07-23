#ifndef NUM_TAB_H
#define NUM_TAB_H
#include<QSpinBox>
#include "Base/Tab_base.h"
#include<functional>

class Num_tab : public QSpinBox, public Tab_base
{
public:
    Num_tab();
    Num_tab(QString name, int index, int min, int max, std::function<void(int)> const &f, QWidget* parent);
    int get_begin_index() override;
    int get_default_index() override;
    void reset() override;
    QString get_name() override;
private:
    int index;
    QString name;
};

#endif // NUM_TAB_H
