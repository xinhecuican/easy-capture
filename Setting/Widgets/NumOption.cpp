#include "NumOption.h"
#include"Manager/config.h"
#include <QStyleOption>
#include <QPainter>

NumOption::NumOption() {

}

NumOption::NumOption(QString name, int index, int min, int max, std::function<void (int)> const &f, QWidget* parent):QSpinBox(parent) {
    setAttribute(Qt::WA_StyledBackground); // 启用 QSS
    connect(this, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), this, f);
    this->name = name;
    this->index = index;
    setMinimum(min);
    setMaximum(max);
    setValue(Config::getConfig<int>(Config::setting(index)));
    setAccelerated(true);
    setWrapping(true);  // 开启循环
    setKeyboardTracking(true);

}

void NumOption::reset() {
    setValue(Config::getConfig<int>(Config::setting(index)));
}

int NumOption::getBeginIndex() {
    return index;
}

int NumOption::getDefaultIndex() {
    return index;
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
