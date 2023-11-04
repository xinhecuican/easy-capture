#include "ComboOption.h"
#include "Manager/config.h"
#include <QDebug>

ComboOption::ComboOption() {

}

ComboOption::ComboOption(QString tab_name, QVector<QString> name, int index, QWidget* parent, bool isString)
    :QComboBox(parent) {
    this->tab_name = tab_name;
    this->index = index;
    _isString = isString;
    names = name;
    setEditable(false);
    for(int i=0; i<name.size(); i++) {
        addItem(name[i]);
    }
    reset();
    connect(this, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, [=](int index){
        isChange = true;
        currentIndex = index;
    });
    saveFunc = [=](int result){
        Config::setConfig(index, result);
        Config::updateConfig((Config::setting)index);
    };
}

ComboOption::ComboOption(QString tab_name, QVector<QString> name, int index,
                         std::function<void (int)> const &f, QWidget* parent, bool isString)
    :QComboBox(parent) {
    this->tab_name = tab_name;
    this->index = index;
    _isString = isString;
    names = name;
    setEditable(false);
    for(int i=0; i<name.size(); i++) {
        addItem(name[i]);
    }
    reset();
    connect(this, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, [=](int index){
        isChange = true;
        currentIndex = index;
    });
    saveFunc = f;
}

QString ComboOption::getName() {
    return tab_name;
}

void ComboOption::reset() {
    if(_isString) {
        QString value = Config::getConfig<QString>(index);
        for(int i=0; i<names.size(); i++) {
            if(value == names[i]) {
                setCurrentIndex(i);
                currentIndex = i;
                break;
            }
        }
    } else {
        currentIndex = Config::getConfig<int>(index);
        setCurrentIndex(Config::getConfig<int>(index));
    }
}

void ComboOption::onSave(){
    if(isChange){
        saveFunc(currentIndex);
    }
}
