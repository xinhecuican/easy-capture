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
    ComboOption(QString tab_name, QVector<QString> name, int index, QWidget* parent, bool isString=false);
    ComboOption(QString tab_name, QVector<QString> name, int index,
                std::function<void (int)> const &f, QWidget* parent, bool isString=false);
    QString getName() override;
    void reset() override;
    void onSave() override;
private:
    QString tab_name;
    int index;
    int currentIndex;
    bool _isString;
    QVector<QString> names;
    std::function<void (int)> saveFunc;
};

#endif // COMBO_TAB_H
