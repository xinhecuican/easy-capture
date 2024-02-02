#include "paintwindow.h"
//#include "ui_paint_window.h"
#include<QBoxLayout>
#include<QScrollArea>
#include "Manager/WindowManager.h"
#include <QToolButton>
#include<QScrollBar>
#include<QDesktopWidget>
#include<QFileDialog>
#include "../Manager/history.h"
#include<QMessageBox>
#include<QListWidget>
#include "Manager/config.h"
#include<QClipboard>
#include "Manager/KeyManager.h"
#include<QScreen>
#include<QMouseEvent>
#include<QButtonGroup>
#include "opencv2/core.hpp"
#include "opencv2/opencv.hpp"
#include "Style_widget/titlebar.h"
#include "Style_widget/framelesshelper.h"
#include <QSound>
#include <QGuiApplication>
#include <QApplication>
#include "Helper/imagehelper.h"
#include "Helper/common.h"

PaintWindow::PaintWindow(QWidget *parent) :
      WindowBase(parent)
//    ui(new Ui::PaintWindow)
{
    //    ui->setupUi(this);
    centralWidget = new QWidget(this);
    centralWidget->setObjectName("centralWidget");
    setCentralWidget(centralWidget);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
//    Titlebar* title_bar = new Titlebar(this);
//    installEventFilter(title_bar);
//    FramelessHelper* helper = new FramelessHelper(this);
//    helper->activateOn(this);
//    helper->setWidgetResizable(true);
//    helper->setWidgetMovable(true);
//    helper->setTitleHeight(30);
//    helper->setRubberBandOnResize(true);
//    setWindowFlags (Qt::FramelessWindowHint);
    setWindowTitle("简截");
    setWindowIcon(QIcon(":/image/avator.png"));
    //    connect(title_bar, &Titlebar::minimize, this, [=]() {
    //        WindowManager::instance()->changeWindow("tray");
    //    });
    menuBar = new QMenuBar(this);
    QWidget* menu_widget = new QWidget(this);
    QVBoxLayout* menu_layout = new QVBoxLayout();
    menu_layout->setMargin(0);
//    menu_layout->addWidget(title_bar);
    menu_layout->addWidget(menuBar);
    menu_widget->setLayout(menu_layout);
    menu_widget->setStyleSheet("background-color: rgb(255, 255, 255)");
    setMenuWidget(menu_widget);

    layout = new QVBoxLayout(centralWidget);

    this->area = new PaintArea(this);
    toolbar = new PaintToolbar(area, this);
    toolbar->showAll();
    addToolBar(Qt::TopToolBarArea, toolbar);

    paint_panel = new QGraphicsView(this->area, this);
    paint_panel->setFrameShape(QFrame::NoFrame);
    paint_panel->setViewportUpdateMode(QGraphicsView::FullViewportUpdate);
    paint_panel->setBackgroundRole(QPalette::Light);
    paint_panel->setAlignment(Qt::AlignCenter);
    paint_panel->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    paint_panel->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    //paint_panel->setWidgetResizable(true);
    paint_panel->setSizeAdjustPolicy(QAbstractScrollArea::AdjustToContents);
    QHBoxLayout* toolbarLayout = new QHBoxLayout();
    toolbarLayout->addWidget(toolbar->getAttributeBar());
    toolbarLayout->addStretch(1);
    layout->addLayout(toolbarLayout);
    layout->addWidget(paint_panel, 1);
    centralWidget->setLayout(layout);
    set_menubar();
    addToolBarBreak();
    setMinimumSize(400, 200);
}

PaintWindow::~PaintWindow() {
    //    delete ui;
}

void PaintWindow::loadKeyEvent(QString name) {
    //    if(!KeyManager::isContainsWindow("PaintWindow"))
    //    {
    KeyManager::instance()->addFunc(this, name, "undo", [=](bool is_enter) {
        if(is_enter) area->getRecorder()->back();
    });
    KeyManager::instance()->addFunc(this, name, "redo", [=](bool is_enter) {
        if(is_enter) area->getRecorder()->forward();
    });
    KeyManager::instance()->addFunc(this, name, "save", [=](bool is_enter) {
        if(is_enter) {
            QString file_name = QFileDialog::getSaveFileName(this,"保存",
                                                             History::instance()->get_last_directory(), "图片(*.bmp *.jpg *.jpeg *.png);;所有文件(*)");
            KeyManager::instance()->clearKeys("PaintWindow");
            if(file_name != "") {
                area->save(ILayerControl::Persist, file_name);
                WindowManager::instance()->changeWindow("tray");
            }
        }
    });
    KeyManager::instance()->addFunc(this, name, "new_capture", [=](bool is_enter) {
        if(is_enter) {
            if(Config::getConfig<int>(Config::capture_mode) == (int)Config::TOTAL_CAPTURE) {
                QTimer::singleShot(200, this, [=]() {
                    QPixmap map = ImageHelper::grabScreen();
                    WindowManager::instance()->changeWindow("PaintWindow", map, ImageHelper::getCurrentScreen()->geometry());
                    if(Config::getConfig<bool>(Config::clip_voice))
                        QSound::play(":/audio/screenshot.wav");
                });

                return;
            } else {
                changeWindowHelper();
            }
            Config::setConfig(Config::capture_mode, Config::RECT_CAPTURE);
        }
    });
    KeyManager::instance()->addFunc(this, name, "delete_shape", [=](bool is_enter) {
        if(is_enter) {
            //                area->delete_shape();
        }
    });
    //    }
}

void PaintWindow::set_menubar() {
    QMenu* file_setting_menu = new QMenu(this);
    file_setting_menu->setTitle(MString::search("{m2Llo9nJ9o}文件"));
    QAction* open_action = new QAction(MString::search("{zA7Twj1QhJ}打开"), file_setting_menu);
    file_setting_menu->addAction(MString::search("{zA7Twj1QhJ}打开"), open_action, [=]() {
        QString file_name = QFileDialog::getOpenFileName(this,
                                                         "打开",
                                                         History::instance()->get_last_directory(),
                                                         "图片(*.bmp *.jpg *.jpeg *.png);;所有文件(*)");
        if(file_name.isEmpty()) {
            return;
        } else {
            QPixmap pixmap;
            if(!pixmap.load(file_name)) {
                qWarning("文件打开失败");
                return;
            }
            QRect rect = pixmap.rect();
            rect.moveTo(0, 0);
            receiveData(pixmap, rect);
        }
    });
    QAction* save_action = new QAction(MString::search("{0EtTa6lKzl}另存为"), file_setting_menu);
    file_setting_menu->addAction(MString::search("{0EtTa6lKzl}另存为"), save_action, [=]() {
        QString file_name = QFileDialog::getSaveFileName(this,
                                                         "保存",
                                                         History::instance()->get_last_directory(),
                                                         "图片(*.bmp *.jpg *.jpeg *.png);;所有文件(*)");
        if(file_name != "") {
            area->save(ILayerControl::Persist, file_name);
            WindowManager::instance()->changeWindow("tray");
        }
    });
    QAction* history_action = new QAction(MString::search("{Mo0LoFqQqT}历史"), menuBar);
    QMenu* history_menu = new QMenu(MString::search("{Mo0LoFqQqT}历史"), menuBar);
    connect(history_action, &QAction::hovered, this, [=]() { //动态生成菜单项
        if(History::instance()->is_change) { //hovered会一直触发，防止重复调用函数导致闪屏
            History::instance()->is_change = false;
            history_menu->clear();
            QList<QAction*> history_actions;
            int history_num = History::instance()->get_history_num();
            for(int i=history_num-1; i>=0; i--) {
                HistoryData data = History::instance()->get(i);
                QString file_name = data.type == ILayerControl::Persist ?
                                                                       data.file_name.mid(data.file_name.lastIndexOf('/')+1) :
                                                                       QDateTime::fromSecsSinceEpoch(data.time).toString("yyyy.MM.dd hh:mm:ss");

                QAction* action = new QAction(history_menu);
                action->setText(file_name);
                connect(action, &QAction::triggered, this, [=]() {
                    QPixmap pixmap;

                    if(!pixmap.load(data.file_name)) {
                        qWarning("文件不存在");
                        return;
                    }
                    QRect rect = pixmap.rect();
                    rect.moveTo(0, 0);
                    //                    area->pic_save = true;//不需要添加记录
                    receiveData(pixmap, rect);
                });
                history_actions.append(action);
            }
            history_menu->addActions(history_actions);
        }
    });
    history_action->setMenu(history_menu);
    file_setting_menu->addAction(history_action);
    menuBar->addMenu(file_setting_menu);
}

void PaintWindow::receiveData(QVariant data1, QVariant data2){
    QPixmap pix = data1.value<QPixmap>();
    QRect rect = data2.value<QRect>();
    if(pix.isNull()){
        pix = QPixmap(100, 100);
    }
    reset();
    area->setImage(pix.toImage());
    area->setSceneRect(-rect.width()/2, -rect.height()/2, rect.width()*2, rect.height()*2);
    paint_panel->update();
    if(Config::getConfig<bool>(Config::auto_copy_to_clipboard)) {
        QClipboard *clip=QApplication::clipboard();
        clip->setPixmap(pix);
    }
    QScreen* screen = ImageHelper::getCurrentScreen();
    QRect geometry = screen->availableGeometry();
    int currentWidth = rect.width() + 100;
    int currentHeight = rect.height() + 140;
    if(currentWidth >= (double)geometry.width()
        || currentHeight >= (double)geometry.height()) {
        showMaximized();
    } else {
        //左上角移动到指定位置，截图越大越向(0, 0)点接近
        move(geometry.x() + (geometry.width()-currentWidth)/2, geometry.y() + (geometry.height() - currentHeight) / 2);
        resize(currentWidth, currentHeight);//设置主窗口大小，否则窗口大小不会变化
    }
    paint_panel->updateGeometry();
//    paint_panel->verticalScrollBar()->setValue(rect.height() / 2);
//    paint_panel->horizontalScrollBar()->setValue(rect.width() / 2);
}

void PaintWindow::closeEvent(QCloseEvent *event) {
    event->ignore();
    //    if(Config::get_config(Config::show_close_dialog))
    //    {
    //        Close_dialog* close_dialog = new Close_dialog(area, this);
    //        connect(close_dialog, &Close_dialog::hide_paint, this, [=](){
    //            WindowManager::instance()->changeWindow("MainWindow");
    //            WindowManager::instance()->hideNow();
    //        });
    //        close_dialog->show();
    //    }
    if(Config::getConfig<bool>(Config::hide_to_tray)) {
        WindowManager::instance()->changeWindow("tray");

    } else {
        WindowManager::instance()->close();
    }
}

void PaintWindow::reset() {
    QDir dir;
    if(!dir.exists("Data/Temp")) {
        dir.mkpath("Data/Temp");
    }
    QDateTime time = QDateTime::currentDateTime();
    QString path = "Data/Temp/" + time.toString("dd_mm_yyyy_hh_mm_ss") + "/";
    if(!dir.exists(path)) {
        dir.mkpath(path);
    }
    path += "main.png";
    if(!area->isSave() && area->isImageValid()) area->save(ILayerControl::Temp, path);
    toolbar->reset();
}

void PaintWindow::onWindowCancel() {

}

void PaintWindow::onWindowSelect() {
    setWindowFlag(Qt::WindowSystemMenuHint, true);
}

void PaintWindow::onPaintWindowClose() {
}

void PaintWindow::onWindowClose() {
    //    delete Style_manager::instance();
    //    delete Recorder::instance();
    //    delete History::instance();
    QClipboard *clip=QApplication::clipboard();
    if(Config::getConfig<bool>(Config::auto_copy_to_clipboard)) {
        clip->clear();
    }
}
