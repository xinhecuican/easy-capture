#include "bool_tab.h"
#include "Manager/config.h"

Bool_tab::Bool_tab()
{

}

Bool_tab::Bool_tab(QString tab_name, int index, std::function<void (bool)> const  &f, QWidget* parent)
    :QRadioButton(parent)
{
    this->tab_name = tab_name;
    this->index = index;
    setChecked(Config::getConfig<int>(Config::setting(index)));
    setCheckable(true);
    setAutoExclusive(false);
    connect(this, &QRadioButton::toggled, this, f);
}



QString Bool_tab::getName()
{
    return tab_name;
}

int Bool_tab::getBeginIndex()
{
    return index;
}

int Bool_tab::getDefaultIndex()
{
    return index;
}

void Bool_tab::reset()
{
    default_option = Config::getConfig<int>(index);
    setChecked(default_option);
}
