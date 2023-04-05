#include "listwrapper.h"
#include <QStyleOption>
#include <QPainter>

ListWrapper::ListWrapper(QWidget* parent) : QWidget(parent) {
    setAttribute(Qt::WA_StyledBackground);
    setAttribute(Qt::WA_Hover);
}

void ListWrapper::paintEvent(QPaintEvent *event) {
    QStyleOption opt;
    opt.init(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}
