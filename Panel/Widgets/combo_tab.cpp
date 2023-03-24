#include "combo_tab.h"
#include "Manager/config.h"
#include <QDebug>

Combo_tab::Combo_tab()
{

}

Combo_tab::Combo_tab(QString tab_name, QVector<QString> name, int begin_index, int end_index,
                     std::function<void (int)> const &f, QWidget* parent)
    :QComboBox(parent)
{
    this->tab_name = tab_name;
    this->begin_index = begin_index;
    this->end_index = end_index;

    setEditable(false);
    for(int i=0; i<name.size(); i++)
    {
        addItem(name[i]);
    }
    reset();
    connect(this, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, f);
}

QString Combo_tab::getName()
{
    return tab_name;
}

int Combo_tab::getBeginIndex()
{
    return begin_index;
}

int Combo_tab::getDefaultIndex()
{
    return begin_index + default_index;
}

void Combo_tab::reset()
{
    if(begin_index == end_index)
    {
        setCurrentIndex(Config::getConfig<int>(begin_index));
        return;
    }
    for(int i=begin_index; i<=end_index; i++)
    {
        if(Config::getConfig<int>(i))
        {
            default_index = i - begin_index;
            setCurrentIndex(default_index);
            return;
        }
    }
}
