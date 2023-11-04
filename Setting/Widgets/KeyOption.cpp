#include "KeyOption.h"
#include "Manager/KeyManager.h"

KeyOption::KeyOption() {

}

KeyOption::KeyOption(int index, QString window_name, QString key_name, QWidget* parent) : QPushButton(parent) {
    this->index = index;
    this->window_name = window_name;
    this->key_name = key_name;
    this->f = [=](QString windowName, QString keyName, QList<int>keys){
        KeyManager::instance()->updateKey(windowName, keyName, keys);
    };
    keys = KeyManager::instance()->getKeys(window_name, key_name);
    set_key_string();
    setCheckable(true);
    connect(this, &QPushButton::clicked, this, [=]() {
        if(!isChecked()) {
            set_key_string();
            KeyManager::instance()->removeKeyListener(this);
        } else {
            setText("");
            temp_keys = QList<int>();
            KeyManager::instance()->addKeyListener(this);
        }
    });
    dirty = false;
}

KeyOption::KeyOption(int index, QString window_name, QString key_name, QWidget* parent, std::function<void (QString, QString, QList<int>)> const &f) : QPushButton(parent) {
    this->index = index;
    this->window_name = window_name;
    this->key_name = key_name;
    this->f = f;
    keys = KeyManager::instance()->getKeys(window_name, key_name);
    set_key_string();
    setCheckable(true);
    connect(this, &QPushButton::clicked, this, [=]() {
        if(!isChecked()) {
            set_key_string();
            KeyManager::instance()->removeKeyListener(this);
        } else {
            setText("");
            temp_keys = QList<int>();
            KeyManager::instance()->addKeyListener(this);
        }
    });
    dirty = false;
}

void KeyOption::getKey(int key) {
    temp_keys.append(key);
    if(text() == "") {
        setText(text() + KeyManager::instance()->keyType[key]);
    } else {
        setText(text() + "+" + KeyManager::instance()->keyType[key]);
    }
}

void KeyOption::keyEnd() {
    KeyManager::instance()->removeKeyListener(this);
    QList<QString> key_names = KeyManager::instance()->detectKeyConflict(window_name, key_name, temp_keys);
    if(key_names.size() != 0) {
        for(int i=0; i<key_names.size(); i++) {
            key_names[i] = window_name + key_names[i];
        }

        emit key_conflict(key_names);
    } else if(temp_keys.size() != 0) {
        setChecked(false);
        isChange = true;
        f(window_name, key_name, temp_keys);
        keys = temp_keys;
        dirty = true;
    } else {
        set_key_string();
        setChecked(false);
    }
}

void KeyOption::set_key_string() {
    setText(KeyManager::instance()->keyType[keys[0]]);
    for(int i=1; i<keys.size(); i++) {
        setText(text() + "+" + KeyManager::instance()->keyType[keys[i]]);
    }
}

void KeyOption::reset() {
    keys = KeyManager::instance()->getKeys(window_name, key_name);;
    set_key_string();
    setChecked(false);
}

QString KeyOption::getName() {
    return window_name + key_name;
}

void KeyOption::restore() {
}

void KeyOption::onSave(){
    if(isChange){
        f(window_name, key_name, keys);
    }
}
