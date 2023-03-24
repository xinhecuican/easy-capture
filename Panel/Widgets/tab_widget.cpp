#include "tab_widget.h"
#include "bool_tab.h"
#include "combo_tab.h"
#include<QLabel>
#include<QDebug>
#include "Helper/mstring.h"
#include "key_tab.h"
#include "Style_widget/spacer.h"
#include<QMessageBox>
#include "num_tab.h"
#include "filechooser.h"
#include "globalkeytab.h"

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
    layout = new QVBoxLayout();
    layout->setContentsMargins(10, 0, 10, 0);
    layout->setSpacing(10);
    layout->setAlignment(Qt::AlignTop);
    base->setLayout(layout);
    setWidget(base);
}

Tab_widget::~Tab_widget()
{

}

void Tab_widget::done()
{

}

void Tab_widget::add_bool_option(QString tab_name, QString name, int index, std::function<void (bool)> const  &f)
{
    Bool_tab* temp = new Bool_tab(tab_name, index, f, base);
    QLabel* label = new QLabel(MString::search(name), this);
    widgets.push_back(temp);
    QHBoxLayout* hlayout = new QHBoxLayout();
    hlayout->addWidget(label);
    hlayout->addWidget(temp);
    layout->addLayout(hlayout);
    update();
}

void Tab_widget::add_combo_option(QString tab_name, QString text, QVector<QString> name, int begin_index, int end_index, const std::function<void (int)> &f)
{
    Combo_tab* temp = new Combo_tab(tab_name, name, begin_index, end_index, f,  base);
    widgets.push_back(temp);
    QHBoxLayout* hlayout = new QHBoxLayout();
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
    QHBoxLayout*  hlayout = new QHBoxLayout();
    hlayout->addWidget(label);
    hlayout->addWidget(element);
    layout->addLayout(hlayout);
}

void Tab_widget::addGlobalKeyOption(int index, QString tabName, QString keyName){
    GlobalKeyTab* element = new GlobalKeyTab(index, tabName, keyName, this);
    widgets.push_back(element);
    QLabel* label = new QLabel(MString::search(tabName), this);
    QHBoxLayout* hlayout = new QHBoxLayout();
    hlayout->addWidget(label);
    hlayout->addWidget(element);
    layout->addLayout(hlayout);
}

void Tab_widget::add_num_option(QString tab_name, int index, QString name,
                                int min, int max, const std::function<void (int)> &f)
{
    Num_tab* element = new Num_tab(tab_name, index, min, max, f, this);
    widgets.push_back(element);
    QLabel* label = new QLabel(MString::search(name), this);
    QHBoxLayout* hlayout = new QHBoxLayout();
    hlayout->addWidget(label);
    hlayout->addWidget(element);
    layout->addLayout(hlayout);
}

void Tab_widget::add_file_option(QString tab_name, QString name, int index, std::function<void(QString)> const &f)
{
    FileChooser* file_chooser = new FileChooser(tab_name, name, index, f, this);
    QLabel* label = new QLabel(MString::search(name), this);
    QHBoxLayout* hlayout = new QHBoxLayout();
    hlayout->addWidget(label);
    hlayout->addWidget(file_chooser);
    widgets.push_back(file_chooser);
    layout->addLayout(hlayout);
}

void Tab_widget::add_spacer(QString text)
{
    Spacer* spacer = new Spacer(text, true, this);
    layout->addWidget(spacer);
}

int Tab_widget::getDefaultIndex(QString name)
{
    for(int i=0; i<widgets.size(); i++)
    {
        if(widgets[i]->getName() == name)
        {
            return widgets[i]->getDefaultIndex();
        }
    }
    return 0;
}

int Tab_widget::getBeginIndex(QString name)
{
    for(int i=0; i<widgets.size(); i++)
    {
        if(widgets[i]->getName() == name)
        {
            return widgets[i]->getBeginIndex();
        }
    }
    return 0;
}

void Tab_widget::reset()
{
    dirty = false;
    for(int i=0; i<widgets.size(); i++)
    {
        widgets[i]->reset();
    }
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

void Tab_widget::add_layout(QLayout *hlayout)
{
    layout->addLayout(hlayout);
}

void Tab_widget::restore(){
    for(TabBase* widget : widgets){
        widget->restore();
    }
}
