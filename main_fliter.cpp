#include "main_fliter.h"
#include "Manager/window_manager.h"
#include<QEvent>
#include<QKeyEvent>
#include "Manager/key_manager.h"
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

Main_fliter* Main_fliter::_instance = NULL;

Main_fliter::Main_fliter()
{
//    global_key_id = GlobalAddAtomA("awake_capture");
//    RegisterHotKey((HWND)this->winId(), global_key_id, MOD_CONTROL, VK_F1);
//    global_capture_id = GlobalAddAtomA("fullscreen_capture");
//    RegisterHotKey((HWND)this->winId(), global_capture_id, MOD_CONTROL, VK_F2);
    //只是延时执行，不是新的线程，相当于回调，因此不用加锁
    //定时删除不使用的Window
    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(window_manager_thread()));
    int time = Config::getConfig<int>(Config::clear_interval);
    timer->start(time * 1000);

    for(int i=Config::capture_mode_begin; i<=Config::capture_mode_end; i++)
    {
        Config::setConfig(i, false);
    }
    Config::setConfig(Config::rect_capture, true);
    setTrayContextMenu();

    Update::instance()->update();
    if(!Config::getConfig<bool>(Config::need_update) && Config::getConfig<int>(Config::update_interval) != -1 &&
            Config::getConfig<int>(Config::last_update_time) + Config::getConfig<int>(Config::update_interval) <
            QDateTime::currentSecsSinceEpoch()/60)
    {
        Update::instance()->check_update();
    }
    checkCrash();
}

Main_fliter::~Main_fliter()
{
}

bool Main_fliter::eventFilter(QObject *o, QEvent *e)
{
    if(e->type() == QEvent::KeyPress)
    {
        QKeyEvent* event = static_cast<QKeyEvent*>(e);
        if(!event->isAutoRepeat())
        {
            Key_manager::key_enter(event->key());
        }
    }
    else if(e->type() == QEvent::KeyRelease)
    {
        QKeyEvent* event = static_cast<QKeyEvent*>(e);
        if(!event->isAutoRepeat())
        {
            Key_manager::key_release(event->key());
        }
    }
    return false;
}

bool Main_fliter::nativeEventFilter(const QByteArray &eventType, void *message, long *result)
{
    MSG* pMsg = reinterpret_cast<MSG*>(message);
    if(pMsg->message == WM_HOTKEY){
        QList<ATOM> keyIds = Key_manager::getGlobalKeyId();
        for(int i=0; i<keyIds.size(); i++){
            if(pMsg->wParam == keyIds[i]){
                switch(i){
                case 0:
                    if(Window_manager::get_now_window() != "Capture_window")
                    {
                        Window_manager::change_window("Capture_window");
                        return true;
                    }
                    break;
                case 1:
                    Window_manager::change_window("tray");
                    QTimer::singleShot(200, this, [=](){
                        QScreen *screen = QGuiApplication::primaryScreen();
                        QPixmap map = screen->grabWindow(0);
                        QString save_path = Config::getConfig<QString>(Config::total_capture_save_path);
                        QString dir_path = save_path.mid(0, save_path.lastIndexOf("/")+1);
                        QDir base_dir = QDir(dir_path);
                        if(!base_dir.exists())
                        {
                            QDir dir;
                            dir.mkpath(dir_path);
                        }
                        QString file_name = QDateTime::currentDateTime().toString("dd_mm_yyyy_hh_mm_ss") + ".png";
                        map.save(save_path + file_name);
                        if(Config::getConfig<bool>(Config::clip_voice))
                            QSound::play(":/audio/screenshot.wav");
                    });
                    break;
                }
                break;
            }
        }
    }
    return false;
}

void Main_fliter::window_manager_thread()
{
    Window_manager::control_window_close();
}

void Main_fliter::stop_timer()
{
    timer->stop();
}

void Main_fliter::start_timer()
{
    int time = Config::getConfig<int>(Config::clear_interval);
    timer->start(time * 1000);
}

bool Main_fliter::is_timer_run()
{
    return timer->isActive();
}

void Main_fliter::setTrayContextMenu()
{
    QMenu* menu = new QMenu(this);
    QAction* close = new QAction(this);
    close->setText(MString::search("{FuBCvgW4BE}关闭"));
    connect(close, &QAction::triggered, this, [=](){
        Window_manager::close();
    });

    QAction* setting = new QAction(this);
    setting->setText(MString::search("{De720dAaY1}设置"));
    connect(setting, &QAction::triggered, this, [=](){
        Window_manager::change_window("Setting");
    });

    QAction* update = new QAction(this);
    update->setText(MString::search("{vOmXOswvHv}检查更新"));
    connect(update, &QAction::triggered, this, [=](){
        Update::instance()->check_update();
    });

    connect(Update::instance(), &Update::updateStateChange, this, [=](Update::update_state_t state){
        switch(state){
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
    for(int i=0; i<actions.size(); i++)
    {
        actions[i]->setData(QVariant(i));
    }
    connect(mode, &QMenu::triggered, this, [=](QAction* action){
        QVariant index_var = action->data();
        int index = index_var.toInt();
        for(int i=Config::capture_mode_begin; i<=Config::capture_mode_end; i++)
        {
            Config::setConfig(i, false);
        }
        Config::setConfig(Config::capture_mode_begin+index, true);
        if(Config::getConfig<bool>(Config::rect_capture) || Config::getConfig<bool>(Config::free_capture)
                || Config::getConfig<bool>(Config::scroll_capture))
            Window_manager::change_window("Capture_window");
        else if(Config::getConfig<bool>(Config::total_capture))
        {
            Window_manager::change_window("tray");
            QTimer::singleShot(200, this, [=](){
                QScreen *screen = QGuiApplication::primaryScreen();
                QPixmap map = screen->grabWindow(0);
                Window_manager::change_window("Paint_window");
                Window_manager::get_window("Paint_window")->
                        set_pic(map, screen->geometry());
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


void Main_fliter::checkCrash(){
    QDir dir("Data/crash");
    if(dir.exists()){
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
            crashUploadProcess->connect(crashUploadProcess, static_cast<void (QProcess::*)(int, QProcess::ExitStatus)>(&QProcess::finished), crashUploadProcess, [=](int exitCode, QProcess::ExitStatus exitStatus){
                qInfo() << "upload end";
                if(exitStatus == QProcess::NormalExit){
                    QFile::remove(file_info.absoluteFilePath());
                    QDir dir("Data/crash");
                    dir.setPath(QCoreApplication::applicationDirPath());
                    dir.cd("Data/crash");
                    dir.removeRecursively();
                    file_info.dir().remove(file_info.fileName());
                }
                crashUploadProcess->deleteLater();
            });
    }
}
