#ifndef NUM_TAB_H
#define NUM_TAB_H
#include<QSpinBox>
#include "Base/TabBase.h"
#include<functional>

class NumOption : public QSpinBox, public TabBase {
    Q_OBJECT
public:
    NumOption();
    NumOption(QString name, int index, int min, int max, std::function<void(int)> const &f, QWidget* parent);
    void paintEvent(QPaintEvent* event) override;
    int getBeginIndex() override;
    int getDefaultIndex() override;
    void reset() override;
    QString getName() override;
private:
    int index;
    QString name;
};

#endif // NUM_TAB_H
