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



QString Bool_tab::get_name()
{
    return tab_name;
}

int Bool_tab::get_begin_index()
{
    return index;
}

int Bool_tab::get_default_index()
{
    return index;
}

void Bool_tab::reset()
{
    default_option = Config::getConfig<int>(index);
    setChecked(default_option);
}
