#ifndef NUM_TAB_H
#define NUM_TAB_H
#include<QSpinBox>
#include "Base/TabBase.h"
#include<functional>

class Num_tab : public QSpinBox, public TabBase
{
public:
    Num_tab();
    Num_tab(QString name, int index, int min, int max, std::function<void(int)> const &f, QWidget* parent);
    int getBeginIndex() override;
    int getDefaultIndex() override;
    void reset() override;
    QString getName() override;
private:
    int index;
    QString name;
};

#endif // NUM_TAB_H
