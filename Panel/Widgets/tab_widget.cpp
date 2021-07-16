#include "tab_widget.h"
#include "bool_tab.h"
#include "combo_tab.h"
#include<QLabel>
#include<QDebug>
#include "Helper/mstring.h"
#include "key_tab.h"
#include "Style_widget/spacer.h"
#include<QMessageBox>

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
    widgets.push_back(temp);
    layout->addWidget(temp);
    update();
}

void Tab_widget::add_combo_option(QString tab_name, QString text, QVector<QString> name, int begin_index, int end_index,
                                  const std::function<void (int)> &f)
{
    Combo_tab* temp = new Combo_tab(tab_name, name, begin_index, end_index, f,  base);
    widgets.push_back(temp);
    QHBoxLayout* hlayout = new QHBoxLayout(base);
    QLabel* label = new QLabel(base);
    label->setText(MString::search(text));
    hlayout->addWidget(label);
    hlayout->addWidget(temp);
    layout->addLayout(hlayout);
    update();
}

void Tab_widget::add_key_option(int index, QString tab_name, QString window_name, QString key_name)
{
    Key_tab* element = new Key_tab(index, window_name, key_name, this);
    widgets.push_back(element);
    connect(element, &Key_tab::key_conflict, this, [=](QList<QString> key_names){
        QString temp_list;
        for(int i=0; i<key_names.size(); i++)
        {
            temp_list.append(MString::search(key_names[i]) + "\n");
        }
        QMessageBox::information(this, "冲突", temp_list);
    });
    QLabel* label = new QLabel(MString::search(tab_name), this);
    QHBoxLayout*  hlayout = new QHBoxLayout(this);
    hlayout->addWidget(label);
    hlayout->addWidget(element);
    layout->addLayout(hlayout);
}

void Tab_widget::add_spacer(QString text)
{
    Spacer* spacer = new Spacer(MString::search(text), true, this);
    layout->addWidget(spacer);
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
