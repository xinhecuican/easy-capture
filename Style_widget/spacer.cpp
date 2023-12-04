#include "spacer.h"
#include<QLabel>
#include<QFrame>
#include "../Helper/mstring.h"
#include <QStyleOption>
#include <QPainter>
#include "../Helper/imagehelper.h"

Spacer::Spacer() {

}

Spacer::Spacer(QString name, bool need_hide_button, QWidget* parent) : QWidget(parent) {
    setAttribute(Qt::WA_StyledBackground);
    this->name = MString::search(name);
    this->parent = parent;
    widgets = PList<QWidget*>();
    show_icon = ImageHelper::getIcon("show");
    hide_icon = ImageHelper::getIcon("hide");
    if(!need_hide_button) {
        hide_button = new QToolButton(this);
        hide_button->setIcon(show_icon);
        connect(hide_button, &QPushButton::clicked, this, [=]() {
            if(state == HIDE) {
                widgets_show();
                hide_button->setIcon(show_icon);
                state = SHOW;
            } else {
                widgets_hide();
                hide_button->setIcon(hide_icon);
                state = HIDE;
            }
        });
        state = SHOW;
    }
    layout = new QVBoxLayout(this);
    QHBoxLayout* line_layout = new QHBoxLayout();
    if(!need_hide_button) {
        line_layout->addWidget(hide_button);
    }
    QLabel* name_label = new QLabel(this->name, this);
    QFrame *line = new QFrame(this);
    line->setObjectName("line");
    line->setFrameShape(QFrame::HLine);
    line->setFrameShadow(QFrame::Plain);
    line_layout->addWidget(name_label, 3);
    line_layout->addWidget(line, 7);
    layout->addLayout(line_layout);
    layout->addSpacing(10);
    setLayout(layout);
}

void Spacer::add_widget(QWidget *widget) {
    layout->addWidget(widget);
    widgets.append(widget);
}

void Spacer::widgets_hide() {
    for(int i=0; i<widgets.size(); i++) {
        widgets[i]->hide();
    }
    parent->update();
}

void Spacer::widgets_show() {
    for(int i=0; i<widgets.size(); i++) {
        widgets[i]->show();
    }
    parent->update();
}

void Spacer::add_layout(QLayout *layout) {
    for(int i=0; i<layout->count(); i++) {
        if(layout->itemAt(i)->layout() != 0) {
            add_layout(layout->itemAt(i)->layout());
        } else if(layout->itemAt(i)->widget() != 0) {
            widgets.append(layout->itemAt(i)->widget());
        }
    }
    this->layout->addLayout(layout);
}

void Spacer::paintEvent(QPaintEvent *event) {
    QStyleOption opt;
    opt.init(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}
