#ifndef COMBO_TAB_H
#define COMBO_TAB_H
#include<QVector>
#include<functional>
#include<QComboBox>
#include "Base/TabBase.h"

class ComboOption : public QComboBox, public TabBase {
    Q_OBJECT
public:
    ComboOption();
    ComboOption(QString tab_name, QVector<QString> name, int index,
                std::function<void (int)> const &f, QWidget* parent, bool isString=false);
    QString getName() override;
    int getBeginIndex() override;
    int getDefaultIndex() override;
    void reset() override;
private:
    QString tab_name;
    int index;
    int defaultIndex;
    bool _isString;
    QVector<QString> names;
};

#endif // COMBO_TAB_H
