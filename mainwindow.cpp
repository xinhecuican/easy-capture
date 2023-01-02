#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "Panel/setting.h"
#include "Manager/window_manager.h"
#include "Manager/key_manager.h"
#include "Helper/mstring.h"
#include "Style_widget/tray.h"
#include "Manager/update.h"
#include "JlCompress.h"

bool MainWindow::is_start = false;

MainWindow::MainWindow(QWidget *parent)
    : Window_base(parent, this, "MainWindow")
    , ui(new Ui::MainWindow)
    , hiding(false)
{
    ui->setupUi(this);

    setAttribute(Qt::WA_DeleteOnClose, true);
    setWindowIcon(QIcon(":/image/avator.png"));
    load_key_event("MainWindow");
    toolbar = new QToolBar();
    toolbar->setMovable(false);     // 设置工具栏不可移动,默认是可移动
    addToolBar(toolbar);

    new_button = new QToolButton(this);
    connect(new_button, &QToolButton::clicked, this, &MainWindow::onCaptureButtonEnter);
    new_button->setText(MString::search("{cR3jOHb9Qw}新建"));
    new_button->setIcon(QIcon(":/image/capture.png"));
    new_button->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    toolbar->addWidget(new_button);

    QToolButton* mode_button = new QToolButton(this);
    mode_button->setText(MString::search(MString::search("{7yUWnx82jI}模式")));
    mode_button->setIcon(QIcon(":/image/mode.png"));
    mode_button->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    mode_button->setPopupMode(QToolButton::InstantPopup);
    for(int i=Config::capture_mode_begin; i<=Config::capture_mode_end; i++)
    {
        Config::setConfig(i, false);
    }
    Config::setConfig(Config::rect_capture, true);
    QMenu* mode_menu = new QMenu(this);
    mode_menu->addAction(MString::search("{OBwjJUhTkh}矩形窗口"));
    mode_menu->addAction(MString::search("{FHFzLMcLYa}全屏"));
    mode_menu->addAction(MString::search("{fnGapBU4vo}自由截图"));
    mode_menu->addAction(MString::search("{ETY295cnab}滚动截屏"));
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
            Config::setConfig(i, false);
        }
        Config::setConfig(Config::capture_mode_begin+index, true);
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

    if(!is_start)
    {
        is_start = true;
        Update::instance()->update();
        if(!Config::getConfig<bool>(Config::need_update) && Config::getConfig<int>(Config::update_interval) != -1 &&
                Config::getConfig<int>(Config::last_update_time) + Config::getConfig<int>(Config::update_interval) <
                QDateTime::currentSecsSinceEpoch()/60)
        {
            Update::instance()->check_update();
        }
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    Window_manager::close();
}

void MainWindow::load_key_event(QString name)
{
    Key_manager::add_func(this, name, "main_capture", [=](QObject* current, bool is_enter){
        if(is_enter)
        {
            onCaptureButtonEnter();
        }
    });
}

void MainWindow::changeEvent(QEvent *event)
{
    if ((event->type() == QEvent::WindowStateChange) && isMinimized())
    {
        Window_manager::change_window("tray");
        event->ignore();
    }
}

void MainWindow::onCaptureButtonEnter()
{
    if(Config::getConfig<bool>(Config::total_capture))
    {
        Window_manager::change_window("tray");
        QTimer::singleShot(200, this, [=](){
            QScreen *screen = QGuiApplication::primaryScreen();
            QPixmap map = screen->grabWindow(0);
            Window_manager::change_window("Paint_window");
            Window_manager::get_window("Paint_window")->
                    set_pic(map, screen->geometry());
        });
        return;
    }
    else
    {
        hide();
        QTimer::singleShot(200, this, [=](){Window_manager::change_window("Capture_window");});
//        Window_manager::change_window("Capture_window");
    }
}
