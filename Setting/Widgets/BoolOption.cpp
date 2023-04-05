#include "BoolOption.h"
#include "Manager/config.h"

BoolOption::BoolOption()
{

}

BoolOption::BoolOption(QString tab_name, int index, std::function<void (bool)> const  &f, QWidget* parent)
    :QRadioButton(parent)
{
    this->tab_name = tab_name;
    this->index = index;
    setChecked(Config::getConfig<int>(Config::setting(index)));
    setCheckable(true);
    setAutoExclusive(false);
    connect(this, &QRadioButton::toggled, this, f);
}



QString BoolOption::getName()
{
    return tab_name;
}

int BoolOption::getBeginIndex()
{
    return index;
}

int BoolOption::getDefaultIndex()
{
    return index;
}

void BoolOption::reset()
{
    default_option = Config::getConfig<int>(index);
    setChecked(default_option);
}
