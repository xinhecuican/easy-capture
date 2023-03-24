#include "globalkeytab.h"
#include "Manager/KeyManager.h"

GlobalKeyTab::GlobalKeyTab()
{

}

GlobalKeyTab::~GlobalKeyTab(){

}

GlobalKeyTab::GlobalKeyTab(int index, QString name, QString keyName, QWidget* parent) : QWidget(parent){
    this->index = index;
    this->name = name;
    this->keyName = keyName;
    key = KeyManager::getGlobalKey(index);
    modKey = KeyManager::getGlobalModKey(index);
    originKey = key;
    originModKey = modKey;
    dirty = false;
    modKeyButton = new QPushButton(this);
    keyButton = new QPushButton(this);
    modKeyButton->setCheckable(true);
    keyButton->setCheckable(true);
    modKeyButton->setText(KeyManager::keyType[modKey]);
    keyButton->setText(KeyManager::keyType[key]);
    listenKey = false;
    icon = new QLabel(this);
    ok = QPixmap(":/image/ok.svg");
    ok = ok.scaled(32, 32);
    cancel = QPixmap(":/image/cancel.svg");
    cancel = cancel.scaled(32, 32);
    icon->setPixmap(KeyManager::isGloablKeyRegistered(index) ? ok : cancel);
    connect(keyButton, &QPushButton::clicked, this, [=](){
        keyButton->setText("");
        modKeyButton->setEnabled(false);
        listenKey = true;
        KeyManager::addKeyListener(this);
    });
    connect(modKeyButton, &QPushButton::clicked, this, [=](){
        modKeyButton->setText("");
        keyButton->setEnabled(false);
        listenKey = false;
        KeyManager::addKeyListener(this);
    });
    root = new QHBoxLayout();
    root->addWidget(modKeyButton);
    root->addWidget(keyButton);
    root->addWidget(icon);
    setLayout(root);
}

void GlobalKeyTab::getKey(int key){
    tempKey = key;
    if(listenKey){
        keyButton->setText(KeyManager::keyType[key]);
    }
    else{
        modKeyButton->setText(KeyManager::keyType[key]);
    }
}

void GlobalKeyTab::keyEnd(){
    KeyManager::remvoeKeyListener(this);
    if(listenKey){
        modKeyButton->setEnabled(true);
        keyButton->setChecked(false);
        if(tempKey == this->key){
            return;
        }
        this->key = tempKey;
        dirty = true;
        KeyManager::addGlobalKey(keyName, modKey, key);
        detectKeyConflict();
    }
    else{
        keyButton->setEnabled(true);
        modKeyButton->setChecked(false);
        if(tempKey == this->modKey){
            return;
        }
        dirty = true;
        if(KeyManager::nativeModKeyCode((Qt::Key)tempKey) == 0){
            modKeyButton->setText("");
            this->modKey = 0;
        }
        else{
            this->modKey = tempKey;
        }
        KeyManager::addGlobalKey(keyName, modKey, key);
        detectKeyConflict();
    }
}

void GlobalKeyTab::detectKeyConflict(){
    ATOM testId = GlobalAddAtomA("easycapture_test");
    bool result = RegisterHotKey((HWND)this->winId(), testId, KeyManager::nativeModKeyCode((Qt::Key)modKey), KeyManager::nativeKeycode((Qt::Key)key));
    icon->setPixmap(result ? ok : cancel);
    UnregisterHotKey((HWND)this->winId(), testId);
}

void GlobalKeyTab::reset()
{

}

int GlobalKeyTab::getBeginIndex()
{
    return 0;
}

int GlobalKeyTab::getDefaultIndex()
{
    return index;
}

QString GlobalKeyTab::getName(){
    return name;
}

void GlobalKeyTab::restore(){
    if(dirty){
        KeyManager::addGlobalKey(keyName, originModKey, originKey);
    }
}
