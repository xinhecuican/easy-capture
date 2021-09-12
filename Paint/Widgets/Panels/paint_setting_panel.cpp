#include "paint_setting_panel.h"
#include<QPushButton>
#include<QColorDialog>
#include<QDebug>
#include<QLabel>
#include "Paint/Widgets/style_manager.h"
#include "Manager/window_manager.h"
#include "Helper/debug.h"
#include "Base/Window_base.h"
#include "Style_widget/spacer.h"
#include<QAction>
#include "Paint/Widgets/Panels/color_selector.h"
#include<QStandardItemModel>
#include<QSpinBox>

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
    layout->setAlignment(Qt::AlignTop);
    init_shape_setting();
    init_pen_setting();
    init_disable_color_setting();
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

void Paint_setting_panel::init_shape_setting()
{
    Spacer* spacer = new Spacer("形状绘制", false, this);
    QGridLayout* shape_layout = new QGridLayout();
    QToolButton* text_button = new QToolButton(this);
    text_button->setIcon(QIcon(":/image/text.png"));
    connect(text_button, &QToolButton::clicked, this, [=](){
        emit paint_shape(TEXT);
    });
    shape_layout->setOriginCorner(Qt::TopLeftCorner);
    shape_layout->addWidget(text_button, 0, 0);
    spacer->add_layout(shape_layout);
    layout->addWidget(spacer);
}

void Paint_setting_panel::init_pen_setting()
{
    Spacer* spacer = new Spacer("{l1DkNbzL1T}画笔设置", false, this);
    QHBoxLayout* color_chooser = new QHBoxLayout();
    QLabel* color_label = new QLabel(MString::search("{fYXepa9pHx}画笔颜色: "), this);
    color_chooser->addWidget(color_label);
    color_button = new QPushButton(this);
    connect(color_button, &QPushButton::clicked, this, [=](){
        QColorDialog dialog;
        QColor begin_color = Style_manager::instance()->get_now().color;
        QColor color = dialog.getColor(begin_color, this, MString::search("{6Of41PN3eL}选择字体颜色"), QColorDialog::ShowAlphaChannel);
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
    /*
    QHBoxLayout* back_chooser = new QHBoxLayout();
    QLabel* back_label = new QLabel(MString::search("{jEFblEjzoU}背景颜色"));
    back_button = new QPushButton(this);
    connect(back_button, &QPushButton::clicked, this, [=](){
        QColorDialog dialog;
        QColor begin_color = Style_manager::instance()->get_now().back_color;
        QColor color = dialog.getColor(begin_color, this, MString::search("{6Of41PN3eL}选择字体颜色"), QColorDialog::ShowAlphaChannel);
        if(begin_color != color)
        {
            Style_manager::instance()->change_back_color(color);
            set_style();
        }
    });
    back_chooser->addWidget(back_label);
    back_chooser->addWidget(back_button);
    spacer->add_layout(back_chooser);

    QHBoxLayout* alpha_chooser = new QHBoxLayout(this);
    QLabel* alpha_label = new QLabel(MString::search("{VfVCbcOMjR}背景透明度"), this);
    QSpinBox* alpha_box = new QSpinBox(this);
    alpha_box->setRange(0, 255);
    alpha_box->setSingleStep(1);
    QSlider* alpha_slider = new QSlider(this);
    alpha_slider->setSingleStep(1);
    alpha_slider->setMinimum(0);
    alpha_slider->setMaximum(255);
    alpha_slider->setOrientation(Qt::Horizontal);
    int alpha = Style_manager::instance()->get_now().back_color.alpha();
    alpha_slider->setValue(alpha);
    alpha_box->setValue(alpha);
    connect(alpha_box, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), this, [=](int value){
        alpha_slider->setValue(value);
        QColor alpha_color = Style_manager::instance()->get_now().back_color;
        alpha_color.setAlpha(value);
        Style_manager::instance()->change_back_color(alpha_color);
        set_style();
    });
    connect(alpha_slider, &QSlider::valueChanged, this, [=](int value){
        alpha_box->setValue(value);
    });
    alpha_chooser->addWidget(alpha_label);
    alpha_chooser->addWidget(alpha_box);
    alpha_chooser->addWidget(alpha_slider);
    spacer->add_layout(alpha_chooser);*/

    QLabel* width_label = new QLabel(MString::search("{zW4GVylzEh}画笔宽度: "), this);
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
    QHBoxLayout* width_layout = new QHBoxLayout();
    width_layout->addWidget(width_label);
    width_layout->addWidget(width_button);
    spacer->add_layout(width_layout);
    layout->addWidget(spacer);
    //layout->addLayout(width_layout);
    set_style();
}

void Paint_setting_panel::init_disable_color_setting()
{
    Spacer* spacer = new Spacer("{pnbWkrTVVw}透明颜色设置", false, this);
    QHBoxLayout* combo_layout = new QHBoxLayout();
    QLabel* combo_label = new QLabel(MString::search("{8RK1prjHf0}透明颜色"), this);
    QComboBox* combo = new QComboBox(this);
    combo->setEditable(false);
    combo->setAutoFillBackground(true);
    connect(combo, static_cast<void (QComboBox::*)(const QString &text)>(&QComboBox::currentIndexChanged),
            this, [=](const QString &text){
        int splitter1 = text.indexOf(',');
        int splitter2 = text.indexOf(',', splitter1+1);
        int red = text.mid(0, splitter1).toInt();
        int green = text.mid(splitter1+1, splitter2-splitter1-1).toInt();
        int blue = text.mid(splitter2+1).toInt();
        combo->setStyleSheet(QString("background-color: rgb(%1,%2,%3)").arg(red).arg(green).arg(blue));

    });
    QToolButton* add_button = new QToolButton(this);
    QToolButton* remove_button = new QToolButton(this);
    connect(add_button, &QToolButton::clicked, this, [=](){
        Color_selector* selector = new Color_selector(this);
        selector->show();
        selector->update();
        connect(selector, &Color_selector::color_select, this, [=](QColor color){
            combo->addItem(QString::number(color.red())+","+QString::number(color.green())
                           +","+QString::number(color.blue()));
            QStandardItemModel* model = qobject_cast<QStandardItemModel*>(combo->model());
            model->item(combo->count()-1)->setBackground(color);
            int red, green, blue;
            color.getRgb(&red, &green, &blue);
            red = red + (red - 127) * 0.5;
            red = (red < 0x00) ? 0xff+red : (red > 0xff) ? red-0xff : red;
            green = green + (green - 127) *  0.5;
            green = (green < 0x00) ? 0xff+green : (green > 0xff) ? green - 0xff : green;
            blue = blue + (blue - 127) * 0.5;
            blue = (blue < 0x00) ? 0xff+blue : (blue > 0xff) ? blue - 0xff : blue;
            model->item(combo->count()-1)->setForeground(QColor(red, green, blue));
            combo->setCurrentIndex(combo->count()-1);
            emit disable_color_change(-1, color);
        });
    });
    connect(remove_button, &QToolButton::clicked, this, [=](){
        if(combo->count()<=0)
        {
            return;
        }
        int index = combo->currentIndex();
        combo->removeItem(index);
        emit disable_color_change(index);
    });
    add_button->setIcon(QIcon(":/image/add.svg"));
    remove_button->setIcon(QIcon(":/image/delete.svg"));
    combo_layout->addWidget(combo_label);
    combo_layout->addWidget(combo);
    combo_layout->addWidget(add_button);
    combo_layout->addWidget(remove_button);
    spacer->add_layout(combo_layout);
    layout->addWidget(spacer);

}

void Paint_setting_panel::init_layer_setting()
{
    Spacer* spacer = new Spacer("{i4yFQ5UXBc}层次管理", false, this);
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
    int a = 0;
    color = Style_manager::instance()->get_now().back_color;
    color.getRgb(&r, &g, &b, &a);
    //back_button->setStyleSheet(QString("background-color: rgba(%1, %2, %3, %4)").arg(r).arg(g).arg(b).arg(a));
    width_button->setEditText(QString::number(Style_manager::instance()->get_now().width));
}
