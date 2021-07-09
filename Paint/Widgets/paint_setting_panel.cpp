#include "paint_setting_panel.h"
#include<QPushButton>
#include<QColorDialog>
#include<QDebug>
#include<QLabel>
#include "style_manager.h"
#include "Manager/window_manager.h"
#include "Helper/debug.h"
#include "Base/Window_base.h"
#include "Style_widget/spacer.h"
#include<QAction>

Paint_setting_panel::Paint_setting_panel()
{
    layout = new QVBoxLayout(this);
    init();
}

Paint_setting_panel::Paint_setting_panel(IControl_layer_change* layer_control, QWidget* parent) : QDockWidget(parent)
{
    this->parent = parent;
    this->layer_control = layer_control;
    setAttribute(Qt::WA_DeleteOnClose);
    layout = new QVBoxLayout(this);//scrollarea的layout
    init_pen_setting();
    init_layer_setting();
    init();

}

Paint_setting_panel::~Paint_setting_panel()
{
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
    setWidget(area);
    /*QWidget* widget = new QWidget(this);
    QHBoxLayout* central_layout = new QHBoxLayout(this);
    central_layout->addWidget(area);
    widget->setLayout(central_layout);
    setWidget(widget);*/
}

void Paint_setting_panel::init_pen_setting()
{
    Spacer* spacer = new Spacer("画笔设置", false, this);
    QHBoxLayout* color_chooser = new QHBoxLayout(this);
    QLabel* color_label = new QLabel("画笔颜色: ", this);
    color_chooser->addWidget(color_label);
    color_button = new QPushButton(this);
    connect(color_button, &QPushButton::clicked, this, [=](){
        QColorDialog dialog;
        QColor begin_color = Style_manager::instance()->get_now().color;
        QColor color = dialog.getColor(begin_color, this, "选择颜色", QColorDialog::ShowAlphaChannel);
        if(begin_color != color)
        {
            Style_manager::instance()->change_color(color);
            set_style();
        }
    });
    color_chooser->addWidget(color_label);
    color_chooser->addWidget(color_button);
    spacer->add_layout(color_chooser);
    //layout->addLayout(color_chooser);

    QLabel* width_label = new QLabel("画笔宽度: ", this);
    width_button = new QComboBox(this);
    QList<QString> width_text = {"2", "3", "4", "6", "8", "10", "12", "14", "16", "18", "20"};
    width_button->addItems(width_text);
    width_button->setEditable(true);
    connect(width_button, static_cast<void (QComboBox::*)(const QString&)>(&QComboBox::currentIndexChanged)
            , this, [=](const QString& text){
        bool success = false;
        int num = text.toInt(&success);
        if(success && num > 0)
        {
            Style_manager::instance()->change_width(num);
        }
    });
    QHBoxLayout* width_layout = new QHBoxLayout(this);
    width_layout->addWidget(width_label);
    width_layout->addWidget(width_button);
    spacer->add_layout(width_layout);
    layout->addWidget(spacer);
    //layout->addLayout(width_layout);
    set_style();
}

void Paint_setting_panel::init_layer_setting()
{
    Spacer* spacer = new Spacer("层次管理", false, this);
    List_widget* widget = new List_widget(this);
    connect(widget, &List_widget::text_change, this, [=](int index, QString text){
        layer_control->layer_rename(index, text);
    });
    connect(widget, &List_widget::button_click, this, [=](int index, List_item::button_type type){
        switch(type)
        {
        case 0:
            layer_control->change_layer_position(index, index-1);break;
        case 1:
            layer_control->change_layer_position(index, index+1);break;
        case 2:
            layer_control->remove_layer(index);break;
        case 3:
            widget->add_widget(layer_control->append_layer());break;
        }
    });
    for(QString str : layer_control->get_layer_name())
    {
        widget->add_widget(str);
    }
    spacer->add_widget(widget);
    layout->addWidget(spacer);
}


void Paint_setting_panel::closeEvent(QCloseEvent *event)
{
    Window_base* window;
    if((window = Window_manager::get_window("Paint_window")) != NULL)
    {
        window->on_paint_panel_close();
    }
    else
    {
        Debug::debug_print_warning("warning: paint_setting_panel close after paint window\nPaint"
                                   "_setting_panel:close_event");
    }
    event->accept();
}


void Paint_setting_panel::set_style()
{
    QColor color = Style_manager::instance()->get_now().color;
    int r = 0, g = 0, b = 0;
    color.getRgb(&r,&g,&b);
    color_button->setStyleSheet(QString("background-color: rgb(%1,%2,%3)").arg(r).arg(g).arg(b));
    width_button->setEditText(QString::number(Style_manager::instance()->get_now().width));
}
