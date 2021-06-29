#include "tab_widget.h"
#include "bool_tab.h"
#include "combo_tab.h"
#include<QLabel>
#include<QDebug>
#include "Helper/mstring.h"

Tab_widget::Tab_widget()
{
    init();
}

Tab_widget::Tab_widget(QString name, QWidget* parent) : QScrollArea(parent)
{
    widget_name = name;
    init();
}

Tab_widget::Tab_widget(QWidget* parent) : QScrollArea(parent)
{
    init();
}

void Tab_widget::init()
{
    dirty = false;
    bool_widgets = QVector<Bool_tab*>();
    combo_option_widgets = QVector<Combo_tab*>();
    setWidgetResizable(true);
    base = new QWidget(this);
    layout = new QVBoxLayout(base);
    layout->addSpacing(20);
    layout->setContentsMargins(10, 0, 10, 0);
    layout->setAlignment(Qt::AlignTop);
    base->setLayout(layout);
    setWidget(base);
}

Tab_widget::~Tab_widget()
{

}

void Tab_widget::add_bool_option(QString tab_name, QString name, int index, std::function<void (bool)> const  &f)
{
    Bool_tab* temp = new Bool_tab(tab_name, name, index, f, base);
    bool_widgets.push_back(temp);
    widgets.push_back(temp);
    layout->addWidget(temp);
    update();
}

void Tab_widget::add_combo_option(QString tab_name, QString text, QVector<QString> name, int begin_index, int end_index,
                                  const std::function<void (int)> &f)
{
    Combo_tab* temp = new Combo_tab(tab_name, name, begin_index, end_index, f,  base);
    combo_option_widgets.push_back(temp);
    widgets.push_back(temp);
    QHBoxLayout* hlayout = new QHBoxLayout(base);
    QLabel* label = new QLabel(base);
    label->setText(MString::search(text));
    hlayout->addWidget(label);
    hlayout->addWidget(temp);
    layout->addLayout(hlayout);
    update();
}

void Tab_widget::add_spacer(QString text)
{
    layout->addSpacing(1);
}

int Tab_widget::get_default_index(QString name)
{
    for(int i=0; i<widgets.size(); i++)
    {
        if(widgets[i]->get_name() == name)
        {
            return widgets[i]->get_default_index();
        }
    }
    /*switch(type)
    {
    case BOOL:
        for(int i=0; i<bool_widgets.size(); i++)
        {
            if(bool_widgets[i]->get_name() == name)
            {
                return bool_widgets[i]->get_default_index();
            }
        }
        break;
    case COMBO:
        for(int i=0; i<combo_option_widgets.size(); i++)
        {
            if(combo_option_widgets[i]->get_name() == name)
            {
                return combo_option_widgets[i]->get_default_index();
            }
        }
        break;
    }*/
    return 0;
}

int Tab_widget::get_begin_index(QString name)
{
    for(int i=0; i<widgets.size(); i++)
    {
        if(widgets[i]->get_name() == name)
        {
            return widgets[i]->get_begin_index();
        }
    }
    /*switch(type)
    {
    case BOOL:
        for(int i=0; i<bool_widgets.size(); i++)
        {
            if(bool_widgets[i]->get_name() == name)
            {
                return bool_widgets[i]->get_begin_index();
            }
        }
        break;
    case COMBO:
        for(int i=0; i<combo_option_widgets.size(); i++)
        {
            if(combo_option_widgets[i]->get_name() == name)
            {
                return combo_option_widgets[i]->get_begin_index();
            }
        }
        break;
    }*/
    return 0;
}

void Tab_widget::reset()
{
    dirty = false;
    for(int i=0; i<widgets.size(); i++)
    {
        widgets[i]->reset();
    }
    /*for(int i=0; i<combo_option_widgets.size(); i++)
    {
        combo_option_widgets[i]->reset();
    }

    for(int i=0; i<bool_widgets.size(); i++)
    {
        bool_widgets[i]->reset();
    }*/
    update();
}

bool Tab_widget::is_dirty()
{
    return dirty;
}

void Tab_widget::set_dirty(bool dirty)
{
    this->dirty = dirty;
}
