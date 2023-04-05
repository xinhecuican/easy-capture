#include "GlobalKeyOption.h"
#include "Manager/KeyManager.h"

GlobalKeyOption::GlobalKeyOption() {

}

GlobalKeyOption::~GlobalKeyOption() {

}

GlobalKeyOption::GlobalKeyOption(int index, QString name, QString keyName, QWidget* parent, std::function<void (QString, int, int)> const &f) : QWidget(parent) {
    this->index = index;
    this->name = name;
    this->keyName = keyName;
    this->f = f;
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
    connect(keyButton, &QPushButton::clicked, this, [=]() {
        if(!keyButton->isChecked()) {
            keyButton->setText(KeyManager::keyType[key]);
            modKeyButton->setEnabled(true);
            KeyManager::removeKeyListener(this);
        } else {
            keyButton->setText("");
            modKeyButton->setEnabled(false);
            listenKey = true;
            KeyManager::addKeyListener(this);
        }
    });
    connect(modKeyButton, &QPushButton::clicked, this, [=]() {
        if(!modKeyButton->isChecked()) {
            modKeyButton->setText(KeyManager::keyType[modKey]);
            keyButton->setEnabled(true);
            KeyManager::removeKeyListener(this);
        } else {
            modKeyButton->setText("");
            keyButton->setEnabled(false);
            listenKey = false;
            KeyManager::addKeyListener(this);
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
        keyButton->setText(KeyManager::keyType[key]);
    } else {
        modKeyButton->setText(KeyManager::keyType[key]);
    }
}

void GlobalKeyOption::keyEnd() {
    KeyManager::removeKeyListener(this);
    if(listenKey) {
        modKeyButton->setEnabled(true);
        keyButton->setChecked(false);
        if(tempKey == this->key) {
            return;
        }
        this->key = tempKey;
        dirty = true;
        f(keyName, modKey, key);
        detectKeyConflict();
    } else {
        keyButton->setEnabled(true);
        modKeyButton->setChecked(false);
        if(tempKey == this->modKey) {
            return;
        }
        dirty = true;
        if(KeyManager::nativeModKeyCode((Qt::Key)tempKey) == 0) {
            modKeyButton->setText("");
            this->modKey = 0;
        } else {
            this->modKey = tempKey;
        }
        f(keyName, modKey, key);
        detectKeyConflict();
    }
}

void GlobalKeyOption::detectKeyConflict() {
    ATOM testId = GlobalAddAtomA("easycapture_test");
    bool result = RegisterHotKey((HWND)this->winId(), testId, KeyManager::nativeModKeyCode((Qt::Key)modKey), KeyManager::nativeKeycode((Qt::Key)key));
    icon->setPixmap(result ? ok : cancel);
    UnregisterHotKey((HWND)this->winId(), testId);
    GlobalDeleteAtom(testId);
}

void GlobalKeyOption::reset() {
    keyButton->setText(KeyManager::keyType[originKey]);
    modKeyButton->setText(KeyManager::keyType[originModKey]);
}

int GlobalKeyOption::getBeginIndex() {
    return 0;
}

int GlobalKeyOption::getDefaultIndex() {
    return index;
}

QString GlobalKeyOption::getName() {
    return name;
}

void GlobalKeyOption::restore() {
}
