#include "paint_setting_panel.h"
#include<QPushButton>
#include<QColorDialog>
#include<QDebug>
#include<QLabel>
#include "Paint/Widgets/style_manager.h"
#include "Manager/WindowManager.h"
#include "Helper/debug.h"
#include "Base/WindowBase.h"
#include "Style_widget/spacer.h"
#include<QAction>
#include "Paint/Widgets/Panels/color_selector.h"
#include<QStandardItemModel>
#include<QSpinBox>
#include "Paint/Widgets/history.h"
#include <QButtonGroup>

Paint_setting_panel* Paint_setting_panel::_instance = NULL;

Paint_setting_panel::Paint_setting_panel()
{
    layout = new QVBoxLayout(this);
    init();
}

Paint_setting_panel::Paint_setting_panel(QWidget* parent) : QDockWidget(parent)
{
    this->parent = parent;
//    setAttribute(Qt::WA_DeleteOnClose);
    layout = new QVBoxLayout();//scrollarea的layout
    layout->setAlignment(Qt::AlignTop);
    init_shape_setting();
    init_pen_setting();
    init_disable_color_setting();
    init_layer_setting();
    init();
}

Paint_setting_panel* Paint_setting_panel::instance(QWidget *parent)
{
    if(_instance == NULL)
    {
        _instance = new Paint_setting_panel(parent);
    }
    return _instance;
}

Paint_setting_panel::~Paint_setting_panel()
{
    _instance = NULL;
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
    hide();
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
//    text_button->setCheckable(true);
//    text_button->setChecked(true);
    text_button->setToolTip("文本框");
    text_button->setIcon(QIcon(":/image/text.png"));
    connect(text_button, &QToolButton::clicked, this, [=](){
        emit paint_shape(TEXT);
    });
//    QToolButton* delete_button = new QToolButton(this);
////    delete_button->setCheckable(true);
//    delete_button->setToolTip("删除形状");
//    delete_button->setIcon(QIcon(":/image/delete.svg"));
//    connect(delete_button, &QToolButton::clicked, this, [=](){
//        emit paint_shape(DELETE_SHAPE);
//    });
    QToolButton* rect_button = new QToolButton(this);
//    rect_button->setCheckable(true);
    rect_button->setToolTip("矩形");
    rect_button->setIcon(QIcon(":/image/rect.png"));
    connect(rect_button, &QToolButton::clicked, this, [=](){
        emit paint_shape(RECTANGLE);
    });
    QToolButton* arrow_button = new QToolButton(this);
//    arrow_button->setCheckable(true);
    arrow_button->setIcon(QIcon(":/image/paint_arrow.png"));
    arrow_button->setToolTip("{D7HSBXWTLj}箭头");
    connect(arrow_button, &QToolButton::clicked, this, [=](){
        emit paint_shape(PAINT_ARROW);
    });
    QToolButton* mosaic_button = new QToolButton(this);
//    mosaic_button->setCheckable(true);
    mosaic_button->setIcon(QIcon(":/image/mosaic.png"));
    mosaic_button->setToolTip("马赛克");
    connect(mosaic_button, &QToolButton::clicked, this, [=](){
       emit paint_shape(BLUR);
    });
//    QButtonGroup* group = new QButtonGroup(this);
//    group->addButton(text_button, 0);
//    group->addButton(delete_button, 1);
//    group->addButton(rect_button, 2);
//    group->addButton(arrow_button, 3);
//    group->addButton(mosaic_button, 4);
//    connect(group, static_cast<void (QButtonGroup::*)(int)>(&QButtonGroup::buttonClicked), this, [=](int id){
//       switch(id)
//       {
//       case 0:emit paint_shape(TEXT);break;
//       case 1:emit paint_shape(DELETE_SHAPE);break;
//       case 2:emit paint_shape(RECTANGLE);break;
//       case 3:emit paint_shape(PAINT_ARROW);break;
//       case 4:emit paint_shape(BLUR);break;
//       }
//    });
    shape_layout->setOriginCorner(Qt::TopLeftCorner);
    shape_layout->addWidget(text_button, 0, 0);
//    shape_layout->addWidget(delete_button, 1, 2);
    shape_layout->addWidget(rect_button, 0, 1);
    shape_layout->addWidget(arrow_button, 0, 2);
    shape_layout->addWidget(mosaic_button, 1, 1);
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
    QList<QString> width_text = {"4", "8", "12", "16", "20", "24", "28", "40", "50"};
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

    QHBoxLayout* save_layout = new QHBoxLayout();
    QLabel* save_label = new QLabel(MString::search("保存透明颜色"), this);
    QComboBox* save_box = new QComboBox(this);
    save_box->setEditable(false);
    save_box->setAutoFillBackground(true);
    QList<QColor> colors = History::instance()->get_color();
    for(int i=0; i<colors.size(); i++)
    {
        save_box->addItem(QString::number(colors[i].red())+","+QString::number(colors[i].green())
                       +","+QString::number(colors[i].blue()));
        QStandardItemModel* model = qobject_cast<QStandardItemModel*>(save_box->model());
        model->item(save_box->count()-1)->setBackground(colors[i]);
        int red, green, blue;
        colors[i].getRgb(&red, &green, &blue);
        red = red + (red - 127) * 0.5;
        red = (red < 0x00) ? 0xff+red : (red > 0xff) ? red-0xff : red;
        green = green + (green - 127) *  0.5;
        green = (green < 0x00) ? 0xff+green : (green > 0xff) ? green - 0xff : green;
        blue = blue + (blue - 127) * 0.5;
        blue = (blue < 0x00) ? 0xff+blue : (blue > 0xff) ? blue - 0xff : blue;
        model->item(save_box->count()-1)->setForeground(QColor(red, green, blue));
        save_box->setCurrentIndex(save_box->count()-1);
    }
    connect(save_box, static_cast<void (QComboBox::*)(const QString &text)>(&QComboBox::currentIndexChanged),
            this, [=](const QString &text){
        int splitter1 = text.indexOf(',');
        int splitter2 = text.indexOf(',', splitter1+1);
        int red = text.mid(0, splitter1).toInt();
        int green = text.mid(splitter1+1, splitter2-splitter1-1).toInt();
        int blue = text.mid(splitter2+1).toInt();
        save_box->setStyleSheet(QString("background-color: rgb(%1,%2,%3)").arg(red).arg(green).arg(blue));
    });
    QToolButton* save_add_button = new QToolButton(this);
    QToolButton* save_remove_button = new QToolButton(this);
    connect(save_add_button, &QToolButton::clicked, this, [=](){
        Color_selector* selector = new Color_selector(this);
        selector->show();
        selector->update();
        connect(selector, &Color_selector::color_select, this, [=](QColor color){
            save_box->addItem(QString::number(color.red())+","+QString::number(color.green())
                           +","+QString::number(color.blue()));
            QStandardItemModel* model = qobject_cast<QStandardItemModel*>(save_box->model());
            model->item(save_box->count()-1)->setBackground(color);
            int red, green, blue;
            color.getRgb(&red, &green, &blue);
            red = red + (red - 127) * 0.5;
            red = (red < 0x00) ? 0xff+red : (red > 0xff) ? red-0xff : red;
            green = green + (green - 127) *  0.5;
            green = (green < 0x00) ? 0xff+green : (green > 0xff) ? green - 0xff : green;
            blue = blue + (blue - 127) * 0.5;
            blue = (blue < 0x00) ? 0xff+blue : (blue > 0xff) ? blue - 0xff : blue;
            model->item(save_box->count()-1)->setForeground(QColor(red, green, blue));
            save_box->setCurrentIndex(save_box->count()-1);
            History::instance()->log_color(color);
            emit saveDisableColorChange(-1, color);
        });
    });
    connect(save_remove_button, &QToolButton::clicked, this, [=](){
        if(save_box->count()<=0)
        {
            return;
        }
        int index = save_box->currentIndex();
        QString str = save_box->itemText(index);
        save_box->removeItem(index);
        QStringList list = str.split(',');
        QColor color(list[0].toInt(), list[1].toInt(), list[2].toInt());
        History::instance()->remove_color(color);
        qDebug() << index;
        emit saveDisableColorChange(index);
    });
    save_add_button->setIcon(QIcon(":/image/add.svg"));
    save_remove_button->setIcon(QIcon(":/image/delete.svg"));
    save_layout->addWidget(save_label);
    save_layout->addWidget(save_box);
    save_layout->addWidget(save_add_button);
    save_layout->addWidget(save_remove_button);
    spacer->add_layout(save_layout);
    spacer->add_layout(combo_layout);
    layout->addWidget(spacer);

}

void Paint_setting_panel::init_layer_setting()
{
    Spacer* spacer = new Spacer("{i4yFQ5UXBc}层次管理", false, this);
    list_widget = new List_widget(this);
    connect(list_widget, &List_widget::text_change, this, [=](int index, QString text){
        emit layer_rename(index, text);
    });
    connect(list_widget, &List_widget::button_click, this, [=](int index, List_item::button_type type){
        switch(type)
        {
        case 0:
            emit change_layer_position(index, index-1);break;
        case 1:
            emit change_layer_position(index, index+1);break;
        case 2:
            emit remove_layer(index);break;
        case 3:
            emit append_layer();
            break;
        }
    });
    emit requestLayersName();
    spacer->add_widget(list_widget);
    layout->addWidget(spacer);
}


void Paint_setting_panel::closeEvent(QCloseEvent *event)
{
    WindowBase* window;
    if((window = WindowManager::getWindow("PaintWindow")) != NULL)
    {
        window->onPaintWindowClose();
    }
    else
    {
        qWarning("warning: paint_setting_panel close after paint window\nPaint"
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

void Paint_setting_panel::receiveLayerName(QString name)
{
    list_widget->add_widget(name);
}

void Paint_setting_panel::receiveLayersName(QStringList name)
{
    for(QString str : name)
    {
        list_widget->add_widget(str);
    }
}
