#include "paint_window.h"
#include "ui_paint_window.h"

Paint_window::Paint_window(QWidget *parent) :
    Window_base(parent, this, "Paint_window"),
    ui(new Ui::Paint_window)
{
    ui->setupUi(this);

    //menuBar设置
    QMenu* file_setting_menu = new QMenu(this);
}

Paint_window::~Paint_window()
{
    delete ui;
}

