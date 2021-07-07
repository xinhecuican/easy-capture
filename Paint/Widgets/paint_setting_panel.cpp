#include "paint_setting_panel.h"
#include<QPushButton>
#include<QColorDialog>
#include<QDebug>
#include<QLabel>
#include "style_manager.h"

Paint_setting_panel::Paint_setting_panel()
{
    layout = new QVBoxLayout(this);
    init();
}

Paint_setting_panel::Paint_setting_panel(QWidget* parent) : QDockWidget(parent)
{
    this->parent = parent;
    layout = new QVBoxLayout(this);//scrollarea的layout
    QHBoxLayout* color_chooser = new QHBoxLayout(this);
    QLabel* color_label = new QLabel("画笔颜色: ", this);
    color_chooser->addWidget(color_label);
    QPushButton* color_button = new QPushButton(this);
    QColor color = Style_manager::instance()->get_now().color;
    int r = 0, g = 0, b = 0;
    color.getRgb(&r,&g,&b);
    color_button->setStyleSheet(QString("background-color: rgb(%1,%2,%3)").arg(r).arg(g).arg(b));
    connect(color_button, &QPushButton::clicked, this, [=](){
        QColorDialog dialog;
        QColor begin_color = Style_manager::instance()->get_now().color;
        QColor color = dialog.getColor(begin_color, this, "选择颜色", QColorDialog::ShowAlphaChannel);
        if(begin_color != color)
        {
            Style_manager::instance()->change_color(color);
        }
    });
    color_chooser->addWidget(color_label);
    color_chooser->addWidget(color_button);
    layout->addLayout(color_chooser);
    init();

}

void Paint_setting_panel::init()
{
    setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    setMinimumSize(100, 100);
    setFeatures(QDockWidget::AllDockWidgetFeatures);
    area = new QScrollArea(this);
    base = new QWidget(this);
    base->setLayout(layout);
    area->setWidget(base);
    area->setFrameShape(QFrame::NoFrame);
    QWidget* widget = new QWidget(this);
    QHBoxLayout* central_layout = new QHBoxLayout(this);
    central_layout->addWidget(area);
    widget->setLayout(central_layout);
    setWidget(widget);
}
