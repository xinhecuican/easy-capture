#include "GlobalKeyOption.h"
#include "Manager/KeyManager.h"

GlobalKeyOption::GlobalKeyOption() {

}

GlobalKeyOption::~GlobalKeyOption() {

}

GlobalKeyOption::GlobalKeyOption(int index, QString name, QString keyName, QWidget* parent) : QWidget(parent) {
    this->index = index;
    this->name = name;
    this->keyName = keyName;

    this->f = [=](QString name, int modKey, int key){
        KeyManager::instance()->updateGlobalKey(name, key, modKey);
    };
    init();
}

GlobalKeyOption::GlobalKeyOption(int index, QString name, QString keyName, QWidget* parent, std::function<void (QString, int, int)> const &f) : QWidget(parent) {
    this->index = index;
    this->name = name;
    this->keyName = keyName;
    this->f = f;
    init();
}

void GlobalKeyOption::init(){
    key = KeyManager::instance()->getGlobalKey(index);
    modKey = KeyManager::instance()->getGlobalModKey(index);
    originKey = key;
    originModKey = modKey;
    modKeyButton = new QPushButton(this);
    keyButton = new QPushButton(this);
    modKeyButton->setCheckable(true);
    keyButton->setCheckable(true);
    modKeyButton->setText(KeyManager::instance()->keyType[modKey]);
    keyButton->setText(KeyManager::instance()->keyType[key]);
    listenKey = false;
    icon = new QLabel(this);
    ok = QPixmap(":/image/ok.svg");
    ok = ok.scaled(32, 32);
    cancel = QPixmap(":/image/cancel.svg");
    cancel = cancel.scaled(32, 32);
    icon->setPixmap(KeyManager::instance()->testGlobalKey(index) ? ok : cancel);
    connect(keyButton, &QPushButton::clicked, this, [=]() {
        if(!keyButton->isChecked()) {
            keyButton->setText(KeyManager::instance()->keyType[key]);
            modKeyButton->setEnabled(true);
            KeyManager::instance()->removeKeyListener(this);
        } else {
            keyButton->setText("");
            modKeyButton->setEnabled(false);
            listenKey = true;
            KeyManager::instance()->addKeyListener(this);
        }
    });
    connect(modKeyButton, &QPushButton::clicked, this, [=]() {
        if(!modKeyButton->isChecked()) {
            modKeyButton->setText(KeyManager::instance()->keyType[modKey]);
            keyButton->setEnabled(true);
            KeyManager::instance()->removeKeyListener(this);
        } else {
            modKeyButton->setText("");
            keyButton->setEnabled(false);
            listenKey = false;
            KeyManager::instance()->addKeyListener(this);
        }
    });
    root = new QHBoxLayout();
    root->addWidget(modKeyButton);
    root->addWidget(keyButton);
    root->addWidget(icon);
    setLayout(root);
}

void GlobalKeyOption::getKey(int key) {
    tempKey = key;
    if(listenKey) {
        keyButton->setText(KeyManager::instance()->keyType[key]);
    } else {
        modKeyButton->setText(KeyManager::instance()->keyType[key]);
    }
}

void GlobalKeyOption::keyEnd() {
    KeyManager::instance()->removeKeyListener(this);
    if(listenKey) {
        modKeyButton->setEnabled(true);
        keyButton->setChecked(false);
        if(tempKey == this->key) {
            return;
        }
        this->key = tempKey;
        isChange = true;
        detectKeyConflict();
    } else {
        keyButton->setEnabled(true);
        modKeyButton->setChecked(false);
        if(tempKey == this->modKey) {
            return;
        }
        if(KeyManager::instance()->nativeModKeyCode((Qt::Key)tempKey) == 0) {
            modKeyButton->setText("");
            this->modKey = 0;
        } else {
            this->modKey = tempKey;
        }
        isChange = true;
        detectKeyConflict();
    }
}

void GlobalKeyOption::detectKeyConflict() {
    if(key == originKey && modKey == originModKey){
        icon->setPixmap(ok);
        return;
    }
    ATOM testId = GlobalAddAtomA("easycapture_test");
    bool result = RegisterHotKey((HWND)this->winId(), testId, KeyManager::instance()->nativeModKeyCode((Qt::Key)modKey), KeyManager::instance()->nativeKeycode((Qt::Key)key));
    icon->setPixmap(result ? ok : cancel);
    UnregisterHotKey((HWND)this->winId(), testId);
    GlobalDeleteAtom(testId);
}

void GlobalKeyOption::reset() {
    key = KeyManager::instance()->getGlobalKey(index);
    modKey = KeyManager::instance()->getGlobalModKey(index);
    originKey = key;
    originModKey = modKey;
    keyButton->setText(KeyManager::instance()->keyType[key]);
    modKeyButton->setText(KeyManager::instance()->keyType[modKey]);
}

QString GlobalKeyOption::getName() {
    return name;
}

void GlobalKeyOption::restore() {
}

void GlobalKeyOption::onSave(){
    if(isChange){
        f(keyName, modKey, key);
    }
}
