#include "num_tab.h"
#include"Manager/config.h"
Num_tab::Num_tab()
{

}

Num_tab::Num_tab(QString name, int index, int min, int max, std::function<void (int)> const &f, QWidget* parent):QSpinBox(parent)
{
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

void Num_tab::reset()
{
    setValue(Config::getConfig<int>(Config::setting(index)));
}

int Num_tab::getBeginIndex()
{
    return index;
}

int Num_tab::getDefaultIndex()
{
    return index;
}

QString Num_tab::getName()
{
    return name;
}
