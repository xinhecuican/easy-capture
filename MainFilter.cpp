#include "MainFilter.h"
#include "Manager/WindowManager.h"
#include<QEvent>
#include<QKeyEvent>
#include "Manager/KeyManager.h"
#include<QTimer>
#include "Manager/config.h"
#include<QMenu>
#include<QApplication>
#include<QScreen>
#include <QDir>
#include <QDateTime>
#include <QSound>
#include "Manager/update.h"
#include "JlCompress.h"
#include "Manager/update.h"
#include "Manager/uimanager.h"
#include "Helper/imagehelper.h"
#include <QDebug>

MainFilter* MainFilter::_instance = NULL;

MainFilter::MainFilter() {
    //    global_key_id = GlobalAddAtomA("awake_capture");
    //    RegisterHotKey((HWND)this->winId(), global_key_id, MOD_CONTROL, VK_F1);
    //    global_capture_id = GlobalAddAtomA("fullscreen_capture");
    //    RegisterHotKey((HWND)this->winId(), global_capture_id, MOD_CONTROL, VK_F2);
    //只是延时执行，不是新的线程，相当于回调，因此不用加锁
    //定时删除不使用的Window
    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(windowMnaagerThread()));
    int time = Config::getConfig<int>(Config::clear_interval);
    timer->start(time * 1000);

    Config::setConfig(Config::capture_mode, Config::RECT_CAPTURE);
    setTrayContextMenu();

    Update::instance()->update();
    if(!Config::getConfig<bool>(Config::need_update) && Config::getConfig<int>(Config::update_interval) != -1 &&
        Config::getConfig<int>(Config::last_update_time) + Config::getConfig<int>(Config::update_interval) <
            QDateTime::currentSecsSinceEpoch()/60) {
        Update::instance()->checkUpdate();
    }
    setStyleSheet(UIManager::instance()->load("tray"));
    checkCrash();
}

MainFilter::~MainFilter() {
}


bool MainFilter::nativeEventFilter(const QByteArray &eventType, void *message, long *result) {
    MSG* pMsg = reinterpret_cast<MSG*>(message);
    if(pMsg->message == WM_HOTKEY) {
        QList<ATOM> keyIds = KeyManager::instance()->getGlobalKeyId();
        for(int i=0; i<keyIds.size(); i++) {
            if(pMsg->wParam == keyIds[i]) {
                switch(i) {
                case 0:
                    Config::setConfig(Config::capture_mode, Config::RECT_CAPTURE);
                    changeWindowHelper();
                    break;
                case 1:
                    if(WindowManager::instance()->getNowWindow() == "tray"){
                        QPixmap map = ImageHelper::grabScreen();
                        QString save_path = Config::getConfig<QString>(Config::total_capture_save_path);
                        QString dir_path = save_path.mid(0, save_path.lastIndexOf("/")+1);
                        QDir base_dir = QDir(dir_path);
                        if(!base_dir.exists()) {
                            QDir dir;
                            dir.mkpath(dir_path);
                        }
                        QString file_name = QDateTime::currentDateTime().toString("dd_mm_yyyy_hh_mm_ss") + ".png";
                        map.save(save_path + file_name);
                        if(Config::getConfig<bool>(Config::clip_voice))
                            QSound::play(":/audio/screenshot.wav");
                    }
                    else{
                        WindowManager::instance()->changeWindow("tray");
                        QTimer::singleShot(200, this, [=]() {
                            QPixmap map = ImageHelper::grabScreen();
                            QString save_path = Config::getConfig<QString>(Config::total_capture_save_path);
                            QString dir_path = save_path.mid(0, save_path.lastIndexOf("/")+1);
                            QDir base_dir = QDir(dir_path);
                            if(!base_dir.exists()) {
                                QDir dir;
                                dir.mkpath(dir_path);
                            }
                            QString file_name = QDateTime::currentDateTime().toString("dd_mm_yyyy_hh_mm_ss") + ".png";
                            map.save(save_path + file_name);
                            if(Config::getConfig<bool>(Config::clip_voice))
                                QSound::play(":/audio/screenshot.wav");
                        });
                    }
                    break;
                case 2:
                    Config::setConfig(Config::capture_mode, Config::SCROLL_CAPTURE);
                    changeWindowHelper();
                    break;
                case 3:
                    if(WindowManager::instance()->getNowWindow() == "CaptureWindow") {
                        WindowManager::instance()->getWindow("CaptureWindow")->startCaptureVideo();
                    }
                    break;
                case 4:
                    if(WindowManager::instance()->getNowWindow() == "CaptureWindow") {
                        WindowManager::instance()->getWindow("CaptureWindow")->pauseCaptureVideo();
                    }
                    break;
                case 5:
                    if(WindowManager::instance()->getNowWindow() == "CaptureWindow") {
                        WindowManager::instance()->getWindow("CaptureWindow")->stopCaptureVideo();
                    }
                    break;

                }
                break;
            }
        }
    }
    return false;
}

void MainFilter::windowMnaagerThread() {
    WindowManager::instance()->controlWindowClose();
}

void MainFilter::stopTimer() {
    timer->stop();
}

void MainFilter::startTimer() {
    int time = Config::getConfig<int>(Config::clear_interval);
    timer->start(time * 1000);
}

bool MainFilter::isTimerRun() {
    return timer->isActive();
}

void MainFilter::setTrayContextMenu() {
    QMenu* menu = new QMenu(this);
    QAction* close = new QAction(this);
    close->setText(MString::search("{FuBCvgW4BE}关闭"));
    connect(close, &QAction::triggered, this, [=]() {
        WindowManager::instance()->close();
    });

    QAction* setting = new QAction(this);
    setting->setText(MString::search("{De720dAaY1}设置"));
    connect(setting, &QAction::triggered, this, [=]() {
        WindowManager::instance()->changeWindow("Setting");
    });

    QAction* update = new QAction(this);
    update->setText(MString::search("{vOmXOswvHv}检查更新"));
    connect(update, &QAction::triggered, this, [=]() {
        Update::instance()->checkUpdate();
    });

    connect(Update::instance(), &Update::updateStateChange, this, [=](Update::update_state_t state) {
        switch(state) {
        case Update::IDLE:
            update->setText(MString::search("{vOmXOswvHv}检查更新"));
            update->setEnabled(true);
            break;
        case Update::CHECKING:
            update->setText(MString::search("{MKFr8c7gW9}正在检查"));
            update->setEnabled(false);
            break;
        case Update::UPDATING:
            update->setText(MString::search("{d6AUxGzHMJ}正在更新"));
            update->setEnabled(false);
            break;
        }
    });

    QMenu* mode = new QMenu("模式", this);
    QAction* rect_mode = new QAction(this);
    mode->addAction(MString::search("{OBwjJUhTkh}矩形窗口"));
    mode->addAction(MString::search("{FHFzLMcLYa}全屏"));
    mode->addAction(MString::search("{fnGapBU4vo}自由截图"));
    mode->addAction(MString::search("{ETY295cnab}滚动截屏"));
    QList<QAction*> actions = mode->actions();
    for(int i=0; i<actions.size(); i++) {
        actions[i]->setData(QVariant(i));
    }
    connect(mode, &QMenu::triggered, this, [=](QAction* action) {
        QVariant index_var = action->data();
        int index = index_var.toInt();
        Config::setConfig(Config::capture_mode, index);
        if(index == (int)Config::RECT_CAPTURE || index == (int)Config::FREE_CAPTURE)
            WindowManager::instance()->changeWindow("CaptureWindow");
        else if(index == (int)Config::SCROLL_CAPTURE)
            WindowManager::instance()->changeWindow("ScrollerWindow");
        else if(index == Config::TOTAL_CAPTURE) {
            WindowManager::instance()->changeWindow("tray");
            QTimer::singleShot(200, this, [=]() {
                QPixmap map = ImageHelper::grabScreen();
                WindowManager::instance()->changeWindow("PaintWindow", map, ImageHelper::getCurrentScreen()->geometry());
                if(Config::getConfig<bool>(Config::clip_voice))
                    QSound::play(":/audio/screenshot.wav");
            });
        }

    });

    menu->addMenu(mode);
    menu->addAction(setting);
    menu->addAction(update);
    menu->addAction(close);
    tray = new Tray(this);
    tray->setContextMenu(menu);
    tray->show();
}


void MainFilter::checkCrash() {
    QDir dir("Data/crash");
    QDir dir2("crashs/");
    if(!dir2.exists()){
        dir2.mkpath("crashs/");
    }
    if(dir.exists()) {
        QString time = QDateTime::currentDateTime().toString("yyyy_MM_dd__hh_mm_ss");
        QString name = "Data/" + time + ".zip";
        QFileInfo file_info(name);
        JlCompress::compressDir(file_info.absoluteFilePath(), dir.absolutePath());
        crashUploadProcess = new QProcess(this);
        QStringList args;
        args << "-u" << "lizilin:Aa123456";
        args << "-F" << "file=@" + file_info.absoluteFilePath();
        args << "121.37.81.150:8201/easycapture";
        crashUploadProcess->setProgram(QCoreApplication::applicationDirPath() + "/curl.exe");
        crashUploadProcess->setArguments(args);
        crashUploadProcess->start();
        crashUploadProcess->connect(crashUploadProcess, static_cast<void (QProcess::*)(int, QProcess::ExitStatus)>(&QProcess::finished), crashUploadProcess, [=](int exitCode, QProcess::ExitStatus exitStatus) {
            qInfo() << "upload end";
            if(exitStatus == QProcess::NormalExit) {
                QFile::remove(file_info.absoluteFilePath());
                QDir dir;
                dir.setPath(QCoreApplication::applicationDirPath());
                dir.cd("Data/crash");
                dir.removeRecursively();
                QDir dir2;
                dir.cd("crashs/");
                dir.removeRecursively();
                file_info.dir().remove(file_info.fileName());
            }
            crashUploadProcess->deleteLater();
        });
    }
}
