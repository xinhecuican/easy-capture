#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "Panel/setting.h"
#include "Manager/window_manager.h"
#include "Manager/key_manager.h"
#include "Helper/mstring.h"
#include "Style_widget/tray.h"


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

    QToolButton* mode_button = new QToolButton(this);
    mode_button->setText(MString::search(MString::search("{7yUWnx82jI}模式")));
    mode_button->setIcon(QIcon(":/image/mode.png"));
    mode_button->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    mode_button->setPopupMode(QToolButton::InstantPopup);
    QMenu* mode_menu = new QMenu(this);
    mode_menu->addAction(MString::search("{OBwjJUhTkh}矩形窗口"));
    mode_menu->addAction(MString::search("{FHFzLMcLYa}全屏"));
    mode_menu->addAction(MString::search("{fnGapBU4vo}自由截图"));
    mode_menu->addAction(MString::search("{ETY295cnab}滚动截屏"));
    mode_menu->addAction(MString::search("活动窗口截屏"));
    QList<QAction*> actions = mode_menu->actions();
    for(int i=0; i<actions.size(); i++)
    {
        actions[i]->setData(QVariant(i));
    }
    connect(mode_menu, &QMenu::triggered, this, [=](QAction* action){
        QVariant index_var = action->data();
        int index = index_var.toInt();
        for(int i=Config::capture_mode_begin; i<=Config::capture_mode_end; i++)
        {
            Config::set_config(i, false);
        }
        Config::set_config(Config::capture_mode_begin+index, true);
    });
    mode_button->setMenu(mode_menu);
    toolbar->addWidget(mode_button);

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

    //显示托盘图标
    Tray* tray = new Tray(this);
    tray->show();
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
