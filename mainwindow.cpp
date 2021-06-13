#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "Panel/setting.h"
#include "Manager/window_manager.h"



MainWindow::MainWindow(QWidget *parent)
    : Window_base(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowTitle("简截");

    toolbar = new QToolBar();
    toolbar->setMovable(false);     // 设置工具栏不可移动,默认是可移动
    addToolBar(toolbar);

    new_button = new QToolButton(this);
    new_button_action = new Capture_button_action(new_button, this);
    new_button->setText("新建");
    new_button->setIcon(QIcon(":/image/capture.png"));
    new_button->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    toolbar->addWidget(new_button);

    QToolButton* setting_button = new QToolButton(this);
    setting_button->setIcon(QIcon(":/image/setting.svg"));
    connect(setting_button, &QToolButton::clicked, this, [=](){
        Window_manager::change_window("Setting");
    });
    toolbar->addWidget(setting_button);
}

MainWindow::~MainWindow()
{
    delete ui;
    delete new_button_action;
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    Config::save_to_config();
}

bool MainWindow::eventFilter(QObject *o, QEvent *e)
{
    if(e->type() == QEvent::KeyPress)
    {
        QKeyEvent* event = static_cast<QKeyEvent*>(e);
    }
    return false;
}
