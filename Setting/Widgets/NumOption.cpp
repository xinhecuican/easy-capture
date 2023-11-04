#include "NumOption.h"
#include"Manager/config.h"
#include <QStyleOption>
#include <QPainter>

NumOption::NumOption() {

}

NumOption::NumOption(QString name, int index, int min, int max, QWidget* parent):QSpinBox(parent) {
    setAttribute(Qt::WA_StyledBackground); // 启用 QSS
    this->name = name;
    this->index = index;
    setMinimum(min);
    setMaximum(max);
    setValue(Config::getConfig<int>(Config::setting(index)));
    currentValue = Config::getConfig<int>(Config::setting(index));
    setAccelerated(true);
    setWrapping(true);  // 开启循环
    setKeyboardTracking(true);
    saveFunc = [=](int result){
        Config::setConfig(index, result);
        Config::updateConfig((Config::setting)index);
    };
    connect(this, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), this, [=](int value){
        isChange = true;
        currentValue = value;
    });
}

NumOption::NumOption(QString name, int index, int min, int max, std::function<void (int)> const &f, QWidget* parent):QSpinBox(parent) {
    setAttribute(Qt::WA_StyledBackground); // 启用 QSS
    this->name = name;
    this->index = index;
    setMinimum(min);
    setMaximum(max);
    setValue(Config::getConfig<int>(Config::setting(index)));
    currentValue = Config::getConfig<int>(Config::setting(index));
    setAccelerated(true);
    setWrapping(true);  // 开启循环
    setKeyboardTracking(true);
    saveFunc = f;
    connect(this, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), this, [=](int value){
        isChange = true;
        currentValue = value;
    });
}

void NumOption::reset() {
    setValue(Config::getConfig<int>(Config::setting(index)));
    currentValue = Config::getConfig<int>(Config::setting(index));
}

QString NumOption::getName() {
    return name;
}

void NumOption::paintEvent(QPaintEvent *event) {
    QStyleOption opt;
    opt.init(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
    QSpinBox::paintEvent(event);
}

void NumOption::onSave(){
    if(isChange){
        saveFunc(currentValue);
    }
}
