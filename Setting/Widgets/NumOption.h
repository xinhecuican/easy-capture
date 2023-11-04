#ifndef NUM_TAB_H
#define NUM_TAB_H
#include<QSpinBox>
#include "Base/TabBase.h"
#include<functional>

class NumOption : public QSpinBox, public TabBase {
    Q_OBJECT
public:
    NumOption();
    NumOption(QString name, int index, int min, int max, QWidget* parent);
    NumOption(QString name, int index, int min, int max, std::function<void(int)> const &f, QWidget* parent);
    void paintEvent(QPaintEvent* event) override;
    void reset() override;
    QString getName() override;
    void onSave() override;
private:
    int index;
    int currentValue;
    QString name;
    std::function<void(int)> saveFunc;
};

#endif // NUM_TAB_H
