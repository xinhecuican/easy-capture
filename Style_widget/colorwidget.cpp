#include "colorwidget.h"
#include "ui_ColorWidget.h"
#include <QToolButton>
#include <QDebug>
#include <QColorDialog>
#include "../Helper/mstring.h"
#include <QStyleOption>
#include <QPainter>

ColorWidget::ColorWidget(QWidget* parent) : QWidget(parent), ui(new Ui::ColorWidget) {
    ui->setupUi(this);
    setAttribute(Qt::WA_StyledBackground);
    default_color = Qt::red;
    ex.setPattern("background-color:\\s*rgba\\(\\s*(\\d+)\\s*,\\s*(\\d+)\\s*,\\s*(\\d+)\\s*,\\s*\\d+\\s*\\)*");
    connect(ui->red, &QToolButton::clicked, this, &ColorWidget::onDefaultClick);
    connect(ui->blue, &QToolButton::clicked, this, &ColorWidget::onDefaultClick);
    connect(ui->yellow, &QToolButton::clicked, this, &ColorWidget::onDefaultClick);
    connect(ui->green, &QToolButton::clicked, this, &ColorWidget::onDefaultClick);
    connect(ui->dark_red, &QToolButton::clicked, this, &ColorWidget::onDefaultClick);
    connect(ui->dark_green, &QToolButton::clicked, this, &ColorWidget::onDefaultClick);
    connect(ui->black, &QToolButton::clicked, this, &ColorWidget::onDefaultClick);
    connect(ui->white, &QToolButton::clicked, this, &ColorWidget::onDefaultClick);
    ui->current->setToolTip(MString::search("{DepMhoivvx}当前颜色"));
    connect(ui->current, &QToolButton::clicked, this, [=]() {
        QColorDialog dialog;
        QColor color = dialog.getColor(color, this, MString::search("{6Of41PN3eL}选择字体颜色"),
                                       QColorDialog::ShowAlphaChannel);
        if(default_color != color) {
            int r = 0, g = 0, b = 0;
            color.getRgb(&r,&g,&b);
            this->default_color.setRgb(r, g, b);
            ui->current->setStyleSheet(QString("background-color: rgba(%1,%2,%3, 1)").arg(r).arg(g).arg(b));
            emit colorChange(default_color);
        }
    });
}

ColorWidget::~ColorWidget() {
    delete ui;
}

void ColorWidget::setCurrentStyle(QColor color) {
    int r = 0, g = 0, b = 0, a = 0;
    default_color = color;
    default_color.getRgb(&r,&g,&b, &a);
    ui->current->setStyleSheet(QString("background-color: rgba(%1,%2,%3, 1)").arg(r).arg(g).arg(b));
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
        ui->current->setStyleSheet(QString("background-color: rgba(%1,%2,%3, 1)").arg(r).arg(g).arg(b));
        emit colorChange(default_color);
    }
}

void ColorWidget::paintEvent(QPaintEvent *event) {
    QStyleOption opt;
    opt.init(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}
