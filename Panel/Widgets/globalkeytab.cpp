#include "globalkeytab.h"
#include "Manager/key_manager.h"

GlobalKeyTab::GlobalKeyTab()
{

}

GlobalKeyTab::~GlobalKeyTab(){

}

GlobalKeyTab::GlobalKeyTab(int index, QString name, QString keyName, QWidget* parent) : QWidget(parent){
    this->index = index;
    this->name = name;
    this->keyName = keyName;
    key = Key_manager::getGlobalKey(index);
    modKey = Key_manager::getGlobalModKey(index);
    originKey = key;
    originModKey = modKey;
    dirty = false;
    modKeyButton = new QPushButton(this);
    keyButton = new QPushButton(this);
    modKeyButton->setCheckable(true);
    keyButton->setCheckable(true);
    modKeyButton->setText(Key_manager::key_type[modKey]);
    keyButton->setText(Key_manager::key_type[key]);
    listenKey = false;
    icon = new QLabel(this);
    ok = QPixmap(":/image/ok.svg");
    ok = ok.scaled(32, 32);
    cancel = QPixmap(":/image/cancel.svg");
    cancel = cancel.scaled(32, 32);
    icon->setPixmap(Key_manager::isGloablKeyRegistered(index) ? ok : cancel);
    connect(keyButton, &QPushButton::clicked, this, [=](){
        keyButton->setText("");
        modKeyButton->setEnabled(false);
        listenKey = true;
        Key_manager::add_key_listener(this);
    });
    connect(modKeyButton, &QPushButton::clicked, this, [=](){
        modKeyButton->setText("");
        keyButton->setEnabled(false);
        listenKey = false;
        Key_manager::add_key_listener(this);
    });
    root = new QHBoxLayout();
    root->addWidget(modKeyButton);
    root->addWidget(keyButton);
    root->addWidget(icon);
    setLayout(root);
}

void GlobalKeyTab::get_key(int key){
    tempKey = key;
    if(listenKey){
        keyButton->setText(Key_manager::key_type[key]);
    }
    else{
        modKeyButton->setText(Key_manager::key_type[key]);
    }
}

void GlobalKeyTab::key_end(){
    Key_manager::remove_key_listener(this);
    if(listenKey){
        modKeyButton->setEnabled(true);
        keyButton->setChecked(false);
        if(tempKey == this->key){
            return;
        }
        this->key = tempKey;
        dirty = true;
        Key_manager::addGlobalKey(keyName, modKey, key);
        detectKeyConflict();
    }
    else{
        keyButton->setEnabled(true);
        modKeyButton->setChecked(false);
        if(tempKey == this->modKey){
            return;
        }
        dirty = true;
        if(Key_manager::nativeModKeyCode((Qt::Key)tempKey) == 0){
            modKeyButton->setText("");
            this->modKey = 0;
        }
        else{
            this->modKey = tempKey;
        }
        Key_manager::addGlobalKey(keyName, modKey, key);
        detectKeyConflict();
    }
}

void GlobalKeyTab::detectKeyConflict(){
    ATOM testId = GlobalAddAtomA("easycapture_test");
    bool result = RegisterHotKey((HWND)this->winId(), testId, Key_manager::nativeModKeyCode((Qt::Key)modKey), Key_manager::nativeKeycode((Qt::Key)key));
    icon->setPixmap(result ? ok : cancel);
    UnregisterHotKey((HWND)this->winId(), testId);
}

void GlobalKeyTab::reset()
{

}

int GlobalKeyTab::get_begin_index()
{
    return 0;
}

int GlobalKeyTab::get_default_index()
{
    return index;
}

QString GlobalKeyTab::get_name(){
    return name;
}

void GlobalKeyTab::restore(){
    if(dirty){
        Key_manager::addGlobalKey(keyName, originModKey, originKey);
    }
}
