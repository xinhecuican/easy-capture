#include "colorwidget.h"
#include <QToolButton>
#include <QDebug>
#include <QColorDialog>
#include "../Helper/mstring.h"
#include <QStyleOption>
#include <QPainter>
#include "../Helper/global.h"
#include "../Helper/common.h"

ColorWidget::ColorWidget(QWidget* parent) : QWidget(parent) {
    setAttribute(Qt::WA_StyledBackground);
    default_color = Qt::red;
    ex.setPattern("background-color:\\s*rgba\\(\\s*(\\d+)\\s*,\\s*(\\d+)\\s*,\\s*(\\d+)\\s*,\\s*\\d+\\s*\\)*");
    layout = new QGridLayout();
    layout->setSpacing(0);
    layout->setContentsMargins(0, 0, 0, 0);
    addDefaultButton(QColor(255, 61, 0), 0, 2);
    addDefaultButton(QColor(2, 136, 209), 0, 3);
    addDefaultButton(QColor(255, 255, 255), 0, 4);
    addDefaultButton(QColor(0, 121, 107), 0, 5);
    addDefaultButton(QColor(251, 192, 45), 1, 2);
    addDefaultButton(QColor(56, 142, 60), 1, 3);
    addDefaultButton(QColor(0, 0, 0), 1, 4);
    addDefaultButton(QColor(123, 31, 162), 1, 5);
    current = new QToolButton(this);
    current->setObjectName("current");
    current->setToolTip(MString::search("{DepMhoivvx}当前颜色"));
    current->setMinimumSize(pt2px(DEFAULT_ICON_SIZE), pt2px(DEFAULT_ICON_SIZE));
    connect(current, &QToolButton::clicked, this, [=]() {
        QColorDialog dialog;
        QColor color = dialog.getColor(color, this, MString::search("{6Of41PN3eL}选择字体颜色"),
                                       QColorDialog::ShowAlphaChannel);
        if(default_color != color) {
            int r = 0, g = 0, b = 0;
            color.getRgb(&r,&g,&b);
            this->default_color.setRgb(r, g, b);
            current->setStyleSheet(QString("background-color: rgba(%1,%2,%3, 1)").arg(r).arg(g).arg(b));
            emit colorChange(default_color);
        }
    });
    layout->addWidget(current, 0, 0, 2, 2);
    setLayout(layout);
}

ColorWidget::~ColorWidget() {

}

void ColorWidget::setCurrentStyle(QColor color) {
    int r = 0, g = 0, b = 0, a = 0;
    default_color = color;
    default_color.getRgb(&r,&g,&b, &a);
    current->setStyleSheet(QString("background-color: rgba(%1,%2,%3, 1)").arg(r).arg(g).arg(b));
    emit colorChange(default_color);
}

void ColorWidget::onDefaultClick() {
    QToolButton* button = qobject_cast<QToolButton*>(QObject::sender());
    int pos = ex.indexIn(button->styleSheet());
    if(pos > -1) {
        int r, g, b;
        r = ex.capturedTexts().at(1).toInt();
        g = ex.capturedTexts().at(2).toInt();
        b = ex.capturedTexts().at(3).toInt();
        default_color.setRgb(r, g, b);
        current->setStyleSheet(QString("background-color: rgba(%1,%2,%3, 1)").arg(r).arg(g).arg(b));
        emit colorChange(default_color);
    }
}

void ColorWidget::paintEvent(QPaintEvent *event) {
    QStyleOption opt;
    opt.init(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}

void ColorWidget::addDefaultButton(QColor color, int x, int y){
    QToolButton* button = new QToolButton(this);
    button->setMaximumSize(pt2px(DEFAULT_ICON_SIZE)/2, pt2px(DEFAULT_ICON_SIZE)/2);
    button->setStyleSheet(QString("background-color: rgba(%1, %2, %3, 1);")
                           .arg(color.red()).arg(color.green()).arg(color.blue()));
    connect(button, &QToolButton::clicked, this, &ColorWidget::onDefaultClick);
    layout->addWidget(button, x, y);
}
