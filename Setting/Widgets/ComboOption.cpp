#include "ComboOption.h"
#include "Manager/config.h"
#include <QDebug>

ComboOption::ComboOption() {

}

ComboOption::ComboOption(QString tab_name, QVector<QString> name, int index,
                         std::function<void (int)> const &f, QWidget* parent)
    :QComboBox(parent) {
    this->tab_name = tab_name;
    this->index = index;

    setEditable(false);
    for(int i=0; i<name.size(); i++) {
        addItem(name[i]);
    }
    reset();
    connect(this, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, f);
}

QString ComboOption::getName() {
    return tab_name;
}

int ComboOption::getBeginIndex() {
    return index;
}

int ComboOption::getDefaultIndex() {
    return index;
}

void ComboOption::reset() {
    setCurrentIndex(Config::getConfig<int>(index));
}
