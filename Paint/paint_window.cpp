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
#include "Style_widget/titlebar.h"
#include "Style_widget/framelesshelper.h"
#include "Paint/Widgets/Layers/baselayer.h"
#include "Paint/Data/History_data.h"

Paint_window::Paint_window(QWidget *parent) :
    Window_base(parent, this, "Paint_window"),
    ui(new Ui::Paint_window)
{
    ui->setupUi(this);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    Titlebar* title_bar = new Titlebar(this);
    installEventFilter(title_bar);
    FramelessHelper* helper = new FramelessHelper(this);
    helper->activateOn(this);
    helper->setWidgetResizable(true);
    helper->setWidgetMovable(true);
    helper->setTitleHeight(30);
    setWindowFlags (Qt::FramelessWindowHint);
    setWindowTitle("简截");
    setWindowIcon(QIcon(":/image/avator.png"));
    connect(title_bar, &Titlebar::minimize, this, [=](){
        Window_manager::hide_now();
    });
    qRegisterMetaType<BaseLayer>("BaseLayer");
    menu_bar = new QMenuBar(this);
    QWidget* menu_widget = new QWidget(this);
    QVBoxLayout* menu_layout = new QVBoxLayout();
    menu_layout->setMargin(0);
    menu_layout->addWidget(title_bar);
    menu_layout->addWidget(menu_bar);
    menu_widget->setLayout(menu_layout);
    menu_widget->setStyleSheet("background-color: rgb(255, 255, 255)");
    setMenuWidget(menu_widget);

    layout = new QHBoxLayout(ui->centralwidget);
    Paint_setting_panel::instance(this);

    this->area = new Paint_area(this);
    paint_panel = new QGraphicsView(this->area, this);
    paint_panel->setFrameShape(QFrame::NoFrame);
    paint_panel->setBackgroundRole(QPalette::Light);
    paint_panel->setAlignment(Qt::AlignCenter);
    paint_panel->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    paint_panel->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    //paint_panel->setWidgetResizable(true);
    paint_panel->setSizeAdjustPolicy(QAbstractScrollArea::AdjustToContents);
    pen_cursor = QCursor(QPixmap(":/image/pencil.png"), 0, 24);

    layout->addWidget(paint_panel);
    ui->centralwidget->setLayout(layout);
    set_menubar();
    set_toolbar();
    initSettingPanel();
    addToolBarBreak();
    addToolBar(Flow_edit_panel::instance());
    Flow_edit_panel::instance()->hide();
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
                if(file_name != "")
                {
                    area->save(History_data::Persist, file_name);
                    Window_manager::hideToMain();
                }
            }
        });
        Key_manager::add_func(name, "new_capture", [=](bool is_enter){
            if(is_enter)
            {
                if(Config::getConfig<bool>(Config::total_capture))
                {
                    QTimer::singleShot(200, this, [=](){
                        Config::setConfig(Config::total_capture, false);
                        QScreen *screen = QGuiApplication::primaryScreen();
                        QPixmap map = screen->grabWindow(0);
                        Window_manager::change_window("Paint_window");
                        Window_manager::get_window("Paint_window")->
                                set_pic(map, QGuiApplication::primaryScreen()->geometry());
                    });

                    return;
                }
                else
                {
                    Window_manager::change_window("Capture_window");
                }
                Config::setConfig(Config::rect_capture, true);
            }
        });
        Key_manager::add_func(name, "delete_shape", [=](bool is_enter){
            if(is_enter)
            {
//                area->delete_shape();
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
        if(file_name != "")
        {
            area->save(History_data::Persist, file_name);
            Window_manager::hideToMain();
        }
    });
    QAction* history_action = new QAction(MString::search("{Mo0LoFqQqT}历史"), menu_bar);
    QMenu* history_menu = new QMenu(MString::search("{Mo0LoFqQqT}历史"), menu_bar);
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
//                    area->pic_save = true;//不需要添加记录
                    set_pic(pixmap, rect);
                });
                history_actions.append(action);
            }
            history_menu->addActions(history_actions);
        }
    });
    history_action->setMenu(history_menu);
    file_setting_menu->addAction(history_action);
    menu_bar->addMenu(file_setting_menu);
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
        actions[i]->setCheckable(true);
        actions[i]->setData(QVariant(i));
    }
    for(int i=Config::capture_mode_begin; i<=Config::capture_mode_end; i++)
    {
        if(Config::getConfig<bool>(i))
        {
            actions[i-Config::capture_mode_begin]->setChecked(true);
            break;
        }
    }
    connect(mode_menu, &QMenu::triggered, this, [=](QAction* action){
        QVariant index_var = action->data();
        int index = index_var.toInt();
        for(int i=0; i<actions.size(); i++)
        {
            actions[i]->setChecked(false);
        }
        action->setChecked(true);
        for(int i=Config::capture_mode_begin; i<=Config::capture_mode_end; i++)
        {
            Config::setConfig(i, false);
        }
        Config::setConfig(Config::capture_mode_begin+index, true);
    });
    mode_button->setMenu(mode_menu);
    ui->toolBar->addWidget(mode_button);

    QToolButton* clipboard_button = new QToolButton(this);
    clipboard_button->setIcon(QIcon(":/image/clipboard.png"));
    clipboard_button->setToolTip(MString::search("{ntbJbEqxwF}复制到剪切板"));
    connect(clipboard_button, &QToolButton::clicked, this, [=](){
        area->save2Clipboard();
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
        if(file_name != "")
        {
            area->save(History_data::Persist, file_name);
            Window_manager::hideToMain();
        }
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

    connect(Recorder::instance(), &Recorder::recordChange, this, [=](){
        if(!Recorder::instance()->undoAvaliable())
        {
            undo_button->setEnabled(false);
        }
        else
        {
            undo_button->setEnabled(true);
        }
        if(!Recorder::instance()->redoAvaliable())
        {
            redo_button->setEnabled(false);
        }
        else
        {
            redo_button->setEnabled(true);
        }
    });

    ui->toolBar->addSeparator();
    paint_button_group = new QButtonGroup(this);
    paint_button_group->setExclusive(true);
    cursor_button = new QToolButton(this);
    cursor_button->setIcon(QIcon(":/image/cursor.png"));
    cursor_button->setToolTip(MString::search("{l4yTU9QXUd}指针"));
    cursor_button->setCursor(QCursor(QPixmap(":/image/cursor.png"), 0, 0));
    cursor_button->setCheckable(true);
    paint_button_group->addButton(cursor_button, 0);
    ui->toolBar->addWidget(cursor_button);

    pencil_button = new QToolButton(this);
    pencil_button->setIcon(QIcon(":/image/pencil.png"));
//    pencil_button->setToolTip();
    pencil_button->setCheckable(true);
    pencil_button->setChecked(true);
    pencil_button->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    pencil_button->setPopupMode(QToolButton::InstantPopup);
    QMenu* pen_menu = new QMenu(this);
    pen_menu->addAction(MString::search("{ndvChZ2O6z}笔"));
    pen_menu->addAction(MString::search("{j54u1kWtCx}荧光笔"));
    QList<QAction*> penactions = pen_menu->actions();
    for(int i=0; i<penactions.size(); i++)
    {
        penactions[i]->setCheckable(true);
        penactions[i]->setData(QVariant(i));
    }
    penactions[0]->setChecked(true);
    connect(pen_menu, &QMenu::triggered, this, [=](QAction* action){
        QVariant index_var = action->data();
        int index = index_var.toInt();
        for(int i=0; i<penactions.size(); i++)
        {
            penactions[i]->setChecked(false);
        }
        action->setChecked(true);
        switch(index)
        {
        case 0:
            Style_manager::instance()->change_pen(Style_manager::default_pencil);
            pen_cursor = QCursor(QPixmap(":/image/pencil.png"), 0, 24);
            break;
        case 1:
            Style_manager::instance()->change_pen(Style_manager::default_highlighter);
            pen_cursor = QCursor(QPixmap(":/image/highlighter_cursor.png"), 0, 24);
            break;
        }
        paint_button_group->buttonClicked(1);
    });
    pencil_button->setMenu(pen_menu);
//    area->setCursor(QCursor(QPixmap(":/image/pencil.png"), 0, 24));
    paint_button_group->addButton(pencil_button, 1);
    ui->toolBar->addWidget(pencil_button);

//    QToolButton* highlighter_button = new QToolButton(this);
//    highlighter_button->setToolTip(MString::search("{j54u1kWtCx}荧光笔"));
//    highlighter_button->setIcon(QIcon(":/image/highlighter.png"));
//    highlighter_button->setCheckable(true);
//    paint_button_group->addButton(highlighter_button, 2);
//    ui->toolBar->addWidget(highlighter_button);

    QToolButton* shape_button = new QToolButton(this);
    shape_button->setToolTip(MString::search("{25JzGpOvFt}形状"));
    shape_button->setIcon(QIcon(":/image/shape.png"));
    shape_button->setCheckable(true);
    paint_button_group->addButton(shape_button, 2);
    ui->toolBar->addWidget(shape_button);

    QToolButton* erase_button = new QToolButton(this);
    erase_button->setToolTip(MString::search("{7cwKObEhcx}擦除"));
    erase_button->setIcon(QIcon(":/image/eraser.png"));
    erase_button->setCheckable(true);
    paint_button_group->addButton(erase_button, 3);
    ui->toolBar->addWidget(erase_button);
    connect(paint_button_group, static_cast<void (QButtonGroup::*)(int)>(&QButtonGroup::buttonClicked)
            , this, [=](int id){
        switch (id)
        {
        case 0:
            area->stateChange(ARROW);
            cursor_button->setChecked(true);
            paint_panel->viewport()->setCursor(QCursor(QPixmap(":/image/cursor.png"), 0, 0));
            break;
        case 1:
            area->stateChange(PAINT);
            pencil_button->setChecked(true);
            paint_panel->viewport()->setCursor(pen_cursor);
            break;
        case 2:
            area->stateChange(SHAPE);
            shape_button->setChecked(true);
            break;
        case 3:
            area->stateChange(ERASE);
            erase_button->setChecked(true);
            paint_panel->viewport()->setCursor(QCursor(QPixmap(":/image/eraser.png"), 4, 20));
            break;
        }
        paint_panel->update();
        Paint_setting_panel::instance()->set_style();
        area->update();
    });
    paint_button_group->buttonClicked(1);

    QToolButton* more_button = new QToolButton(this);
    more_button->setToolTip(MString::search("{dAIlC0hPf5}更多画图选项"));
    more_button->setIcon(QIcon(":/image/more.png"));
    connect(more_button, &QToolButton::clicked, this, &Paint_window::showSettingPanel);
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
    area->setPic(pix, rect);//在这个函数中还设置了paint_panel的大小
    paint_panel->update();
    paint_button_group->buttonClicked(1);
    if(Config::getConfig<bool>(Config::auto_copy_to_clipboard))
    {
        QClipboard *clip=QApplication::clipboard();
        clip->setPixmap(pix);
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

    paint_panel->verticalScrollBar()->setValue(rect.height() / 2);
    paint_panel->horizontalScrollBar()->setValue(rect.width() / 2);
//    paint_panel->verticalScrollBar()->setSliderPosition(rect.height() / 2);
//    paint_panel->horizontalScrollBar()->setSliderPosition(rect.width() / 2);

}

void Paint_window::closeEvent(QCloseEvent *event)
{
    event->ignore();
//    if(Config::get_config(Config::show_close_dialog))
//    {
//        Close_dialog* close_dialog = new Close_dialog(area, this);
//        connect(close_dialog, &Close_dialog::hide_paint, this, [=](){
//            Window_manager::change_window("MainWindow");
//            Window_manager::hide_now();
//        });
//        close_dialog->show();
//    }
    if(Config::getConfig<bool>(Config::hide_to_tray))
    {
        Window_manager::hideToMain();

    }
    else
    {
        Window_manager::close();
    }
}

void Paint_window::reset()
{
    if(area->needSave())
    {
        QDir dir;
        if(!dir.exists("Data/Temp"))
        {
            dir.mkpath("Data/Temp");
        }
        QDateTime time = QDateTime::currentDateTime();
        QString path = "Data/Temp/" + time.toString("dd_mm_yyyy_hh_mm_ss") + "/";
        if(!dir.exists(path))
        {
            dir.mkpath(path);
        }
        path += "main.png";
        area->save(History_data::Editable, path);
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
}

void Paint_window::on_window_close()
{
    delete Style_manager::instance();
    delete Recorder::instance();
    delete History::instance();
    delete Paint_setting_panel::instance();
    QClipboard *clip=QApplication::clipboard();
    if(Config::getConfig<bool>(Config::auto_copy_to_clipboard))
    {
        clip->clear();
    }
}

void Paint_window::showSettingPanel()
{
    if(Paint_setting_panel::instance()->isVisible())
    {
        Paint_setting_panel::instance()->hide();
    }
    else
    {
        addDockWidget(Qt::RightDockWidgetArea, Paint_setting_panel::instance());
        Paint_setting_panel::instance()->show();
    }
}

void Paint_window::initSettingPanel()
{
    connect(Paint_setting_panel::instance(), &Paint_setting_panel::paint_shape, this,
            [=](SHAPE_TYPE type){
        if(type == DELETE_SHAPE)
        {
            area->deleteShape();
        }
        else
        {
            area->paintShape(type);
            paint_button_group->buttonClicked(2);
        }
    });
}
