#include "paint_window.h"
#include "ui_paint_window.h"
#include<QBoxLayout>
#include<QScrollArea>
#include "Manager/window_manager.h"
#include <QToolButton>
#include<QScrollBar>
#include<QDesktopWidget>
#include<QFileDialog>
#include "Widgets/recoder.h"
#include "Widgets/paint_setting_panel.h"

Paint_window::Paint_window(QWidget *parent) :
    Window_base(parent, this, "Paint_window"),
    ui(new Ui::Paint_window)
{
    ui->setupUi(this);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    this->area = new Paint_area(this);
    layout = new QHBoxLayout(ui->centralwidget);
    paint_panel = new QScrollArea(this);
    paint_panel->setWidget(area);
    paint_panel->setFrameShape(QFrame::NoFrame);
    paint_panel->setBackgroundRole(QPalette::Light);
    paint_panel->setAlignment(Qt::AlignCenter);
    //paint_panel->setWidgetResizable(true);
    paint_panel->setSizeAdjustPolicy(QAbstractScrollArea::AdjustToContents);

    layout->addWidget(paint_panel);
    ui->centralwidget->setLayout(layout);
    set_menubar();
    set_toolbar();

}

Paint_window::~Paint_window()
{
    delete ui;
    delete new_button_action;
}

void Paint_window::set_menubar()
{
    QMenu* file_setting_menu = new QMenu(this);
    file_setting_menu->setTitle("文件");
    QAction* open_action = new QAction("打开", file_setting_menu);
    file_setting_menu->addAction("打开", open_action, [=](){
        QString file_name = QFileDialog::getOpenFileName(this,
                                     "打开",
                                     "C:/",
                                     "图片(*.bmp *.jpg *.jpeg *.png)");
        if(file_name.isEmpty())
        {
            return;
        }
        else
        {
            QPixmap pixmap;
            pixmap.load(file_name);
            QRect rect = pixmap.rect();
            rect.moveTo(0, 0);
            set_pic(pixmap, rect);
        }
    });
    QAction* save_action = new QAction("另存为", file_setting_menu);
    file_setting_menu->addAction("另存为", save_action, [=](){

    });
    ui->menuBar->addMenu(file_setting_menu);
}

void Paint_window::set_toolbar()
{
    QToolButton* new_button = new QToolButton(this);
    new_button_action = new Capture_button_action(new_button, this);
    new_button->setText(MString::search("{cR3jOHb9Qw}新建"));
    new_button->setIcon(QIcon(":/image/capture.png"));
    new_button->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    ui->toolBar->addWidget(new_button);

    QToolButton* undo_button = new QToolButton(this);
    undo_button->setIcon(QIcon(":/image/undo.png"));
    connect(undo_button, &QPushButton::clicked, this, [=](){
        Recoder::instance()->back();
    });
    ui->toolBar->addWidget(undo_button);
    QToolButton* redo_button = new QToolButton(this);
    redo_button->setIcon(QIcon(":/image/redo.png"));
    connect(redo_button, &QPushButton::clicked, this, [=](){
        Recoder::instance()->forward();
    });
    ui->toolBar->addWidget(redo_button);

    QToolButton* more_button = new QToolButton(this);
    more_button->setIcon(QIcon(":/image/more.png"));
    connect(more_button, &QToolButton::clicked, this, [=](){
        Paint_setting_panel* panel = new Paint_setting_panel(this);
        addDockWidget(Qt::RightDockWidgetArea, panel);
    });
    ui->toolBar->addWidget(more_button);
}

void Paint_window::set_pic(QPixmap pix, QRect rect)
{
    area->reset();
    area->set_picture(pix, rect);//在这个函数中还设置了paint_panel的大小
    resize(rect.width()+100, rect.height()+140);//设置主窗口大小，否则窗口大小不会变化
    QDesktopWidget* desktop = QApplication::desktop();
    //左上角移动到指定位置，截图越大越向(0, 0)点接近
    move(pos().x() * (1-(rect.width()/(double)desktop->screenGeometry().width()>1
                        ?1:rect.width()/(double)desktop->screenGeometry().width()
    )), pos().y() * (1 - (rect.height()/(double)desktop->screenGeometry().height()>1
                         ?1:rect.height()/(double)desktop->screenGeometry().height())));
    paint_panel->verticalScrollBar()->setSliderPosition(140);
    paint_panel->horizontalScrollBar()->setSliderPosition(140);
}

void Paint_window::closeEvent(QCloseEvent *event)
{
    Window_manager::close();
}

