#include "TabWidget.h"
#include "BoolOption.h"
#include "ComboOption.h"
#include<QLabel>
#include<QDebug>
#include "Helper/mstring.h"
#include "KeyOption.h"
#include "Style_widget/spacer.h"
#include<QMessageBox>
#include "NumOption.h"
#include "filechooser.h"
#include "GlobalKeyOption.h"
#include <QPainter>
#include "listwrapper.h"

TabWidget::TabWidget() {
    init();
}

TabWidget::TabWidget(QString name, QWidget* parent) : QScrollArea(parent) {
    widget_name = name;
    init();
}

TabWidget::TabWidget(QWidget* parent) : QScrollArea(parent) {
    init();
}

void TabWidget::init() {
    this->setAttribute(Qt::WA_StyledBackground);
    dirty = false;
    setFrameShape(QFrame::NoFrame);
    setWidgetResizable(true);
    base = new QWidget(this);
    base->setObjectName("base");
    layout = new QVBoxLayout();
    layout->setContentsMargins(10, 0, 10, 0);
    layout->setSpacing(15);
    layout->setAlignment(Qt::AlignTop);
    base->setLayout(layout);
    setWidget(base);
}

TabWidget::~TabWidget() {

}

QString TabWidget::getName() {
    return widget_name;
}

void TabWidget::add_bool_option(QString tab_name, QString name, int index, std::function<void (bool)> const  &f) {
    BoolOption* temp = new BoolOption(tab_name, index, f, base);
    QLabel* label = new QLabel(MString::search(name), this);
    widgets.push_back(temp);
    QHBoxLayout* hlayout = new QHBoxLayout();
    hlayout->addWidget(label);
    hlayout->addWidget(temp);
    ListWrapper* wrapper = new ListWrapper(this);
    wrapper->setLayout(hlayout);
    layout->addWidget(wrapper);
    update();
}

void TabWidget::add_combo_option(QString tab_name, QString text, QVector<QString> name, int index, const std::function<void (int)> &f, bool isString) {
    ComboOption* temp = new ComboOption(tab_name, name, index, f,  base, isString);
    widgets.push_back(temp);
    QHBoxLayout* hlayout = new QHBoxLayout();
    QLabel* label = new QLabel(base);
    label->setText(MString::search(text));
    hlayout->addWidget(label);
    hlayout->addWidget(temp);
    ListWrapper* wrapper = new ListWrapper(this);
    wrapper->setLayout(hlayout);
    layout->addWidget(wrapper);
    update();
}

void TabWidget::add_key_option(int index, QString tab_name, QString window_name, QString key_name, std::function<void (QString, QString, QList<int>)> const &f) {
    KeyOption* element = new KeyOption(index, window_name, key_name, this, f);
    widgets.push_back(element);
    connect(element, &KeyOption::key_conflict, this, [=](QList<QString> key_names) {
        QString temp_list;
        for(int i=0; i<key_names.size(); i++) {
            temp_list.append(MString::search(key_names[i]) + "\n");
        }
        QMessageBox::information(this, "冲突", temp_list);
    });
    QLabel* label = new QLabel(MString::search(tab_name), this);
    QHBoxLayout*  hlayout = new QHBoxLayout();
    hlayout->addWidget(label);
    hlayout->addWidget(element);
    ListWrapper* wrapper = new ListWrapper(this);
    wrapper->setLayout(hlayout);
    layout->addWidget(wrapper);
}

void TabWidget::addGlobalKeyOption(int index, QString tabName, QString keyName, std::function<void (QString, int, int)> const &f) {
    GlobalKeyOption* element = new GlobalKeyOption(index, tabName, keyName, this, f);
    widgets.push_back(element);
    QLabel* label = new QLabel(MString::search(tabName), this);
    QHBoxLayout* hlayout = new QHBoxLayout();
    hlayout->addWidget(label);
    hlayout->addWidget(element);
    ListWrapper* wrapper = new ListWrapper(this);
    wrapper->setLayout(hlayout);
    layout->addWidget(wrapper);
}

void TabWidget::add_num_option(QString tab_name, int index, QString name,
                               int min, int max, const std::function<void (int)> &f) {
    NumOption* element = new NumOption(tab_name, index, min, max, f, this);
    widgets.push_back(element);
    QLabel* label = new QLabel(MString::search(name), this);
    QHBoxLayout* hlayout = new QHBoxLayout();
    hlayout->addWidget(label);
    hlayout->addWidget(element);
    ListWrapper* wrapper = new ListWrapper(this);
    wrapper->setLayout(hlayout);
    layout->addWidget(wrapper);
}

void TabWidget::add_file_option(QString tab_name, QString name, int index, std::function<void(QString)> const &f) {
    FileChooser* file_chooser = new FileChooser(tab_name, name, index, f, this);
    QLabel* label = new QLabel(MString::search(name), this);
    QHBoxLayout* hlayout = new QHBoxLayout();
    hlayout->addWidget(label);
    hlayout->addWidget(file_chooser);
    widgets.push_back(file_chooser);
    ListWrapper* wrapper = new ListWrapper(this);
    wrapper->setLayout(hlayout);
    layout->addWidget(wrapper);
}

void TabWidget::add_spacer(QString text) {
    Spacer* spacer = new Spacer(text, true, this);
    layout->addWidget(spacer);
}

void TabWidget::addText(QString text, QString objectName) {
    QLabel* label = new QLabel(MString::search(text), this);
    label->setObjectName(objectName);
    layout->addWidget(label);
}

int TabWidget::getDefaultIndex(QString name) {
    for(int i=0; i<widgets.size(); i++) {
        if(widgets[i]->getName() == name) {
            return widgets[i]->getDefaultIndex();
        }
    }
    return 0;
}

int TabWidget::getBeginIndex(QString name) {
    for(int i=0; i<widgets.size(); i++) {
        if(widgets[i]->getName() == name) {
            return widgets[i]->getBeginIndex();
        }
    }
    return 0;
}

void TabWidget::reset() {
    dirty = false;
    for(int i=0; i<widgets.size(); i++) {
        widgets[i]->reset();
    }
    update();
}

bool TabWidget::is_dirty() {
    return dirty;
}

void TabWidget::set_dirty(bool dirty) {
    this->dirty = dirty;
}

void TabWidget::add_layout(QLayout *hlayout) {
    layout->addLayout(hlayout);
}

void TabWidget::restore() {
    for(TabBase* widget : widgets) {
        widget->restore();
    }
}
