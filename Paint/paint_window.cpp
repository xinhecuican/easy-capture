#include "paint_window.h"
#include "ui_paint_window.h"
#include<QBoxLayout>
#include<QScrollArea>
#include "Manager/window_manager.h"
#include <QToolButton>
#include<QScrollBar>
#include<QDesktopWidget>
#include<QFileDialog>
#include "Widgets/recorder.h"
#include "Widgets/style_manager.h"
#include "Widgets/history.h"
#include<QMessageBox>
#include<QListWidget>
#include "Manager/config.h"
#include<QClipboard>
#include "Manager/key_manager.h"
#include<QScreen>
#include<QMouseEvent>
#include<QButtonGroup>
#include "Paint/Widgets/Panels/close_dialog.h"
#include "Paint/Widgets/Panels/paint_setting_panel.h"
#include "Paint/Widgets/Panels/flow_edit_panel.h"
#include "opencv2/core.hpp"
#include "opencv2/opencv.hpp"

Paint_window::Paint_window(QWidget *parent) :
    Window_base(parent, this, "Paint_window"),
    ui(new Ui::Paint_window)
{
    ui->setupUi(this);


    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    this->area = new Paint_area(this);
    layout = new QHBoxLayout(ui->centralwidget);
    paint_setting_panel = NULL;
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
    load_key_event("Paint_window");
}

Paint_window::~Paint_window()
{
    delete ui;
    delete new_button_action;
}

void Paint_window::load_key_event(QString name)
{
    if(!Key_manager::is_contains_window("Paint_window"))
    {
        Key_manager::add_func(name, "undo", [=](bool is_enter){if(is_enter) Recorder::instance()->back();});
        Key_manager::add_func(name, "redo", [=](bool is_enter){if(is_enter) Recorder::instance()->forward();});
        Key_manager::add_func(name, "save", [=](bool is_enter){
            if(is_enter)
            {
                QString file_name = QFileDialog::getSaveFileName(this,"保存",
                History::instance()->get_last_directory(), "图片(*.bmp *.jpg *.jpeg *.png);;所有文件(*)");
                area->save(file_name);
            }
        });
        Key_manager::add_func(name, "new_capture", [=](bool is_enter){
            if(is_enter)
            {
                if(Config::get_config(Config::total_capture))
                {
                    Config::set_config(Config::total_capture, false);
                    Window_manager::change_window("Paint_window");
                    QScreen *screen = QGuiApplication::primaryScreen();
                    QPixmap map = screen->grabWindow(0);
                    Window_manager::get_window("Paint_window")->
                            set_pic(map, QApplication::desktop()->screenGeometry());
                    return;
                }
                else
                {
                    Window_manager::change_window("Capture_window");
                }
                Config::set_config(Config::rect_capture, true);
            }
        });
    }
}

void Paint_window::set_menubar()
{
    QMenu* file_setting_menu = new QMenu(this);
    file_setting_menu->setTitle(MString::search("{m2Llo9nJ9o}文件"));
    QAction* open_action = new QAction(MString::search("{zA7Twj1QhJ}打开"), file_setting_menu);
    file_setting_menu->addAction(MString::search("{zA7Twj1QhJ}打开"), open_action, [=](){
        QString file_name = QFileDialog::getOpenFileName(this,
                                     "打开",
                                     History::instance()->get_last_directory(),
                                     "图片(*.bmp *.jpg *.jpeg *.png);;所有文件(*)");
        if(file_name.isEmpty())
        {
            return;
        }
        else
        {
            QPixmap pixmap;
            if(!pixmap.load(file_name))
            {
                Debug::show_error_message("文件打开失败");
                return;
            }
            QRect rect = pixmap.rect();
            rect.moveTo(0, 0);
            set_pic(pixmap, rect);
        }
    });
    QAction* save_action = new QAction(MString::search("{0EtTa6lKzl}另存为"), file_setting_menu);
    file_setting_menu->addAction(MString::search("{0EtTa6lKzl}另存为"), save_action, [=](){
        QString file_name = QFileDialog::getSaveFileName(this,
                                                         "保存",
                                                         History::instance()->get_last_directory(),
                                                         "图片(*.bmp *.jpg *.jpeg *.png);;所有文件(*)");
        area->save(file_name);
    });
    QAction* history_action = new QAction(MString::search("{Mo0LoFqQqT}历史"), ui->menuBar);
    QMenu* history_menu = new QMenu(MString::search("{Mo0LoFqQqT}历史"), ui->menuBar);
    connect(history_action, &QAction::hovered, this, [=](){//动态生成菜单项
        if(History::instance()->is_change)//hovered会一直触发，防止重复调用函数导致闪屏
        {
            History::instance()->is_change = false;
            history_menu->clear();
            QList<QAction*> history_actions;
            int history_num = History::instance()->get_history_num();
            for(int i=history_num-1; i>=0; i--)
            {
                History_data data = History::instance()->get(i);
                QString file_name = data.type == History_data::Persist ?
                            data.file_name.mid(data.file_name.lastIndexOf('/')+1) :
                            QDateTime::fromSecsSinceEpoch(data.time).toString("yyyy.MM.dd hh:mm:ss");

                QAction* action = new QAction(history_menu);
                action->setText(file_name);
                connect(action, &QAction::triggered, this, [=](){
                    QPixmap pixmap;

                    if(!pixmap.load(data.file_name))
                    {
                        Debug::show_error_message("文件不存在");
                        return;
                    }
                    QRect rect = pixmap.rect();
                    rect.moveTo(0, 0);
                    area->pic_save = true;//不需要添加记录
                    set_pic(pixmap, rect);
                });
                history_actions.append(action);
            }
            history_menu->addActions(history_actions);
        }
    });
    history_action->setMenu(history_menu);
    file_setting_menu->addAction(history_action);
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
    ui->toolBar->addWidget(mode_button);

    QToolButton* clipboard_button = new QToolButton(this);
    clipboard_button->setIcon(QIcon(":/image/clipboard.png"));
    clipboard_button->setToolTip(MString::search("{ntbJbEqxwF}复制到剪切板"));
    connect(clipboard_button, &QToolButton::clicked, this, [=](){
        QClipboard *clip=QApplication::clipboard();
        clip->setImage(area->get_image());
    });
    ui->toolBar->addWidget(clipboard_button);

    QToolButton* save_button = new QToolButton(this);
    save_button->setIcon(QIcon(":/image/save.png"));
    save_button->setToolTip(MString::search("{pJqTHhEQdb}保存"));
    connect(save_button, &QToolButton::clicked, this, [=](){
        QString file_name = QFileDialog::getSaveFileName(this,
                                                         "保存",
                                                         History::instance()->get_last_directory(),
                                                         "图片(*.bmp *.jpg *.jpeg *.png);;所有文件(*)");
        area->save(file_name);
    });
    ui->toolBar->addWidget(save_button);
    ui->toolBar->addSeparator();

    QToolButton* undo_button = new QToolButton(this);
    undo_button->setIcon(QIcon(":/image/undo.png"));
    undo_button->setToolTip(MString::search("{h5KymvIMTN}撤销"));
    connect(undo_button, &QPushButton::clicked, this, [=](){
        Recorder::instance()->back();
    });
    ui->toolBar->addWidget(undo_button);
    QToolButton* redo_button = new QToolButton(this);
    redo_button->setToolTip(MString::search("{a7CaC7NOL5}恢复"));
    redo_button->setIcon(QIcon(":/image/redo.png"));
    connect(redo_button, &QPushButton::clicked, this, [=](){
        Recorder::instance()->forward();
    });
    ui->toolBar->addWidget(redo_button);

    ui->toolBar->addSeparator();
    QButtonGroup* paint_button_group = new QButtonGroup(this);
    paint_button_group->setExclusive(true);
    QToolButton* cursor_button = new QToolButton(this);
    cursor_button->setIcon(QIcon(":/image/cursor.png"));
    cursor_button->setToolTip(MString::search("指针"));
    cursor_button->setCursor(QCursor(QPixmap(":/image/cursor.png")));
    cursor_button->setCheckable(true);
    paint_button_group->addButton(cursor_button, 0);
    ui->toolBar->addWidget(cursor_button);

    pencil_button = new QToolButton(this);
    pencil_button->setIcon(QIcon(":/image/pencil.png"));
    pencil_button->setToolTip(MString::search("{ndvChZ2O6z}笔"));
    pencil_button->setCheckable(true);
    pencil_button->setChecked(true);
    area->setCursor(QCursor(QPixmap(":/image/pencil.png"), 0, 24));
    paint_button_group->addButton(pencil_button, 1);
    ui->toolBar->addWidget(pencil_button);

    QToolButton* highlighter_button = new QToolButton(this);
    highlighter_button->setToolTip(MString::search("{j54u1kWtCx}荧光笔"));
    highlighter_button->setIcon(QIcon(":/image/highlighter.png"));
    highlighter_button->setCheckable(true);
    paint_button_group->addButton(highlighter_button, 2);
    ui->toolBar->addWidget(highlighter_button);

    QToolButton* erase_button = new QToolButton(this);
    erase_button->setToolTip(MString::search("{7cwKObEhcx}擦除"));
    erase_button->setIcon(QIcon(":/image/eraser.png"));
    erase_button->setCheckable(true);
    paint_button_group->addButton(erase_button, 3);
    ui->toolBar->addWidget(erase_button);
    connect(paint_button_group, static_cast<void (QButtonGroup::*)(int)>(&QButtonGroup::buttonClicked)
            , this, [=](int id){
        area->using_erase(false);
        switch (id)
        {
        case 0:
            area->set_paintable(false);
            area->setCursor(QCursor(QPixmap(":/image/cursor.png"), 0, 0));
            break;
        case 1:
            area->set_paintable(true);
            pencil_button->setChecked(true);
            Style_manager::instance()->change_pen(Style_manager::default_pencil);
            area->setCursor(QCursor(QPixmap(":/image/pencil.png"), 0, 24));
            break;
        case 2:
            area->set_paintable(true);
            highlighter_button->setChecked(true);
            Style_manager::instance()->change_pen(Style_manager::default_highlighter);
            area->setCursor(QCursor(QPixmap(":/image/highlighter_cursor.png"), 0, 24));
            break;
        case 3:
            area->using_erase(true);
            area->set_paintable(false);
            area->setCursor(QCursor(QPixmap(":/image/eraser.png"), 4, 20));
            break;
        }
        if(paint_setting_panel != NULL)
        {
            paint_setting_panel->set_style();
        }
        area->update();
    });

    QToolButton* more_button = new QToolButton(this);
    more_button->setToolTip(MString::search("{dAIlC0hPf5}更多画图选项"));
    more_button->setIcon(QIcon(":/image/more.png"));
    connect(more_button, &QToolButton::clicked, this, [=](){
        if(paint_setting_panel == NULL)
        {
            paint_setting_panel = new Paint_setting_panel(this);
            connect(paint_setting_panel, &Paint_setting_panel::disable_color_change, this,
                    [=](int index, QColor color=QColor()){
                area->set_disable_color(index, color);
            });
            connect(paint_setting_panel, &Paint_setting_panel::paint_shape, this,
                    [=](shape_type type){
                cursor_button->setChecked(true);
                area->setCursor(QCursor(QPixmap(":/image/cursor.png"), 0, 0));
                area->paint_shape(type);
            });
            addDockWidget(Qt::RightDockWidgetArea, paint_setting_panel);
        }
        else
        {
            paint_setting_panel->close();
        }
    });
    ui->toolBar->addWidget(more_button);

    QToolButton* setting_button = new QToolButton(this);
    setting_button->setToolTip(MString::search("{dTUpYwhZRM}设置"));
    setting_button->setIcon(QIcon(":/image/setting.svg"));
    connect(setting_button, &QToolButton::clicked, this, [=](){
        Window_manager::open_window("Setting");
    });
    ui->toolBar->addWidget(setting_button);
    ui->toolBar->addSeparator();

//    QToolButton* test_button = new QToolButton(this);
//    test_button->setText("test");
//    connect(test_button, &QToolButton::clicked, this, [=](){
//        QString file_name = QFileDialog::getOpenFileName(this,
//                                                         "保存",
//                                                         History::instance()->get_last_directory(),
//                                                         "图片(*.bmp *.jpg *.jpeg *.png);;所有文件(*)");
//        QString file_name2 = QFileDialog::getOpenFileName(this,
//                                                          "保存",
//                                                          History::instance()->get_last_directory(),
//                                                          "图片(*.bmp *.jpg *.jpeg *.png);;所有文件(*)");
//        cv::Mat image1 = cv::imread(file_name.toStdString());
//        cv::Mat image2 = cv::imread(file_name2.toStdString());
//        cv::resize(image1, image1, cv::Size(image2.cols, image1.rows));
//        cv::Mat ans_image = cv::Mat(image1.rows+image2.rows, image1.cols, CV_8UC3);
//        image1.copyTo(ans_image(cv::Rect(0, 0, image1.cols, image1.rows)));
//        image2.copyTo(ans_image(cv::Rect(0, image1.rows, image1.cols, image2.rows)));

//        vector<int> compression_params;
//        compression_params.push_back(cv::IMWRITE_PNG_COMPRESSION);
//        compression_params.push_back(5);    // 无压缩png.
//        compression_params.push_back(IMWRITE_PNG_STRATEGY);
//        compression_params.push_back(IMWRITE_PNG_STRATEGY_FILTERED);
//        imwrite("F:/dinfo/test.png", ans_image, compression_params);
//    });
//    ui->toolBar->addWidget(test_button);
}

void Paint_window::set_pic(QPixmap pix, QRect rect)
{
    reset();
    area->resize(rect.width() * 2, rect.height() * 2);
    area->set_picture(pix, rect);//在这个函数中还设置了paint_panel的大小
    if(Config::get_config(Config::auto_copy_to_clipboard))
    {
        QClipboard *clip=QApplication::clipboard();
        clip->setPixmap(pix);
    }
    else
    {
        QClipboard *clip=QApplication::clipboard();
        clip->clear();
    }
    QScreen* screen = QGuiApplication::primaryScreen();
    if(rect.width()+100 >= (double)screen->geometry().width()
            || rect.height()+140 >= (double)screen->geometry().height())
    {
        showMaximized();
    }
    else
    {
        resize(rect.width()+100, rect.height()+140);//设置主窗口大小，否则窗口大小不会变化
        //左上角移动到指定位置，截图越大越向(0, 0)点接近
        move(pos().x() * (1-(rect.width()/(double)screen->geometry().width()>1
                            ?1:rect.width()/(double)screen->geometry().width()
        )), pos().y() * (1 - (rect.height()/(double)screen->geometry().height()>1
                             ?1:rect.height()/(double)screen->geometry().height())));
    }


    paint_panel->verticalScrollBar()->setSliderPosition(rect.height() / 2);
    paint_panel->horizontalScrollBar()->setSliderPosition(rect.width() / 2);

}

void Paint_window::closeEvent(QCloseEvent *event)
{
    if(Config::get_config(Config::show_close_dialog))
    {
        Close_dialog* close_dialog = new Close_dialog(area, this);
        event->ignore();
        connect(close_dialog, &Close_dialog::hide_paint, this, [=](){
            QTimer::singleShot(200, this, [=](){
                Window_manager::change_window("MainWindow");
                Window_manager::hide_now();
            });
        });
        close_dialog->show();
    }
    else if(Config::get_config(Config::hide_to_tray))
    {
        QTimer::singleShot(200, this, [=](){
            Window_manager::change_window("MainWindow");
            Window_manager::hide_now();
        });
    }
    else
    {
        Window_manager::close();
    }
}

void Paint_window::reset()
{
    if(area->contain_picture() && !area->pic_save)
    {
        area->save_temp();
    }
    pencil_button->setChecked(true);
    area->reset();
    Style_manager::instance()->reset();
    Recorder::instance()->reset();
}

void Paint_window::on_window_cancal()
{
}

void Paint_window::on_paint_panel_close()
{
    paint_setting_panel = NULL;
}

void Paint_window::on_window_close()
{
    delete Style_manager::instance();
    delete Recorder::instance();
    delete History::instance();
    delete Flow_edit_panel::instance();
    QClipboard *clip=QApplication::clipboard();
    clip->clear();
}

QString  Paint_window::append_layer()
{
    return area->create_layer();
}

void Paint_window::remove_layer(int index)
{
    area->remove_layer(index);
}

void Paint_window::layer_rename(int index, QString after_name)
{
    area->set_name(index, after_name);
}

void Paint_window::change_layer_position(int before_index, int after_index)
{
    if(before_index > after_index)
    {
        area->drop_layer(before_index);
    }
    else
    {
        area->raise_layer(before_index);
    }
}

QStringList Paint_window::get_layer_name()
{
    return area->layers_name();
}

void Paint_window::changeEvent(QEvent *event)
{
    if ((event->type() == QEvent::WindowStateChange) && isMinimized())
    {
        Window_manager::hide_now();
        event->ignore();
    }
}
