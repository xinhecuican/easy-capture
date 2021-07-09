#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "Panel/setting.h"
#include "Manager/window_manager.h"
#include "Manager/key_manager.h"
#include "Helper/mstring.h"



MainWindow::MainWindow(QWidget *parent)
    : Window_base(parent, this, "MainWindow")
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);


    toolbar = new QToolBar();
    toolbar->setMovable(false);     // 设置工具栏不可移动,默认是可移动
    addToolBar(toolbar);

    new_button = new QToolButton(this);
    new_button_action = new Capture_button_action(new_button, this);
    new_button->setText(MString::search("{cR3jOHb9Qw}新建"));
    new_button->setIcon(QIcon(":/image/capture.png"));
    new_button->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    toolbar->addWidget(new_button);

    QToolButton* setting_button = new QToolButton(this);
    setting_button->setIcon(QIcon(":/image/setting.svg"));
    connect(setting_button, &QToolButton::clicked, this, [=](){
        Window_manager::change_window("Setting");
    });
    toolbar->addWidget(setting_button);
    setWindowFlag(Qt::WindowMinimizeButtonHint);
    setAttribute(Qt::WA_DeleteOnClose, true);

    //只是延时执行，不是新的线程，相当于回调，因此不用加锁
    //定时删除不使用的Window
    QTimer* timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(window_manager_thread()));
    int time = Config::get_config(Config::clear_interval);
    timer->start(time * 1000);
}

MainWindow::~MainWindow()
{
    delete ui;
    delete new_button_action;
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    Key_manager::save();
}

bool MainWindow::eventFilter(QObject *o, QEvent *e)
{
    if(e->type() == QEvent::KeyPress)
    {
        QKeyEvent* event = static_cast<QKeyEvent*>(e);
        if(!event->isAutoRepeat())
        {
            Key_manager::key_enter(event->key());
        }
        return true;
    }
    else if(e->type() == QEvent::KeyRelease)
    {
        QKeyEvent* event = static_cast<QKeyEvent*>(e);
        if(!event->isAutoRepeat())
        {
            Key_manager::key_release(event->key());
        }
        return true;
    }
    return false;
}

void MainWindow::window_manager_thread()
{
    Window_manager::control_window_close();
}
