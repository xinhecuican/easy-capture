#include "attributetoolbar.h"
#include <QStyleOption>
#include <QPainter>

AttributeToolbar::AttributeToolbar(QWidget* parent) : QWidget(parent)
{
    setAttribute(Qt::WA_StyledBackground);
    setMaximumHeight(48);
    layout = new QHBoxLayout();
    layout->setMargin(0);
    layout->setSpacing(0);
    layout->setContentsMargins(0, 0, 0, 0);
    setLayout(layout);
}

void AttributeToolbar::add(QWidget *widget)
{
    widget->show();
    widget->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    layout->addWidget(widget);
    adjustSize();
//    actions.append(QToolBar::addWidget(widget));
}

void AttributeToolbar::removeAll()
{
    QLayoutItem *child;
    while ((child = layout->takeAt(0)) != nullptr)
    {
        if(child->widget())
        {
            layout->removeWidget(child->widget());
            child->widget()->hide();
        }
    }
    update();
}

void AttributeToolbar::paintEvent(QPaintEvent *event)
{
    QStyleOption opt;
    opt.init(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}
