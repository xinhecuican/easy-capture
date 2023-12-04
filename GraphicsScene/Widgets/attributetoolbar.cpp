#include "attributetoolbar.h"
#include <QStyleOption>
#include <QPainter>

AttributeToolbar::AttributeToolbar(QWidget* parent) : QWidget(parent) {
    setAttribute(Qt::WA_StyledBackground);
    layout = new QHBoxLayout();
    layout->setSpacing(3);
    layout->setContentsMargins(3, 2, 3, 2);
    setLayout(layout);
}

void AttributeToolbar::addWidget(QWidget *widget) {
    if(!containWidget(widget)){
        widgets.append(widget);
        widget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    }
    widget->show();
    layout->addWidget(widget);
    adjustSize();
    //    actions.append(QToolBar::addWidget(widget));
}

void AttributeToolbar::removeAll() {
    QLayoutItem *child;
    while ((child = layout->takeAt(0)) != nullptr) {
        if(child->widget()) {
            layout->removeWidget(child->widget());
            child->widget()->hide();
        }
        if(child->layout()){
            auto childLayout = child->layout();
            layout->removeItem(childLayout);
        }
    }
    update();
}

void AttributeToolbar::hideAll(){
    QLayoutItem *child;
    while ((child = layout->takeAt(0)) != nullptr) {
        if(child->widget()) {
            child->widget()->hide();
        }
    }
    update();
}

void AttributeToolbar::paintEvent(QPaintEvent *event) {
    QStyleOption opt;
    opt.init(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}

void AttributeToolbar::setAttriLayout(QLayout *layout){
    removeAll();
    this->layout->addLayout(layout, 1);
    adjustSize();
}

void AttributeToolbar::showWidget(QWidget *widget){
    removeAll();
    addWidget(widget);
}

bool AttributeToolbar::containWidget(QWidget* widget){
    return widgets.contains(widget);
}
