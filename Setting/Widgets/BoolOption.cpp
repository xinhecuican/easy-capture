#include "BoolOption.h"
#include "Manager/config.h"

BoolOption::BoolOption()
{

}

BoolOption::BoolOption(QString tab_name, int index, QWidget* parent)
    :QRadioButton(parent)
{
    this->tab_name = tab_name;
    this->index = index;
    currentOption = Config::getConfig<int>(Config::setting(index));
    setChecked(currentOption);
    setCheckable(true);
    setAutoExclusive(false);
    connect(this, &QRadioButton::toggled, this, [=](bool isChecked){
        isChange = true;
        currentOption = isChecked;
    });
    saveFunc = [=](bool result){
        Config::setConfig(index, result);
        Config::updateConfig((Config::setting)index);
    };
}

BoolOption::BoolOption(QString tab_name, int index, std::function<void (bool)> const  &f, QWidget* parent)
    :QRadioButton(parent)
{
    this->tab_name = tab_name;
    this->index = index;
    currentOption = Config::getConfig<int>(Config::setting(index));
    setChecked(currentOption);
    setCheckable(true);
    setAutoExclusive(false);
    connect(this, &QRadioButton::toggled, this, [=](bool isChecked){
        isChange = true;
        currentOption = isChecked;
    });
    saveFunc = f;
}



QString BoolOption::getName()
{
    return tab_name;
}

void BoolOption::reset()
{
    currentOption = Config::getConfig<int>(Config::setting(index));
    setChecked(currentOption);
}

void BoolOption::onSave(){
    if(isChange){
        saveFunc(currentOption);
    }
}
