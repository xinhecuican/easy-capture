#include "KeyOption.h"
#include "Manager/KeyManager.h"

KeyOption::KeyOption() {

}

KeyOption::KeyOption(int index, QString window_name, QString key_name, QWidget* parent, std::function<void (QString, QString, QList<int>)> const &f) : QPushButton(parent) {
    this->index = index;
    this->window_name = window_name;
    this->key_name = key_name;
    this->f = f;
    keys = KeyManager::getKeys(window_name, key_name);
    set_key_string();
    setCheckable(true);
    connect(this, &QPushButton::clicked, this, [=]() {
        if(!isChecked()) {
            set_key_string();
            KeyManager::removeKeyListener(this);
        } else {
            setText("");
            temp_keys = QList<int>();
            KeyManager::addKeyListener(this);
        }
    });
    dirty = false;
}

void KeyOption::getKey(int key) {
    temp_keys.append(key);
    if(text() == "") {
        setText(text() + KeyManager::keyType[key]);
    } else {
        setText(text() + "+" + KeyManager::keyType[key]);
    }
}

void KeyOption::keyEnd() {
    KeyManager::removeKeyListener(this);
    QList<QString> key_names = KeyManager::detectKeyConflict(window_name, key_name, temp_keys);
    if(key_names.size() != 0) {
        for(int i=0; i<key_names.size(); i++) {
            key_names[i] = window_name + key_names[i];
        }

        emit key_conflict(key_names);
    } else if(temp_keys.size() != 0) {
        setChecked(false);
        f(window_name, key_name, temp_keys);
        keys = temp_keys;
        dirty = true;
    } else {
        set_key_string();
        setChecked(false);
    }
}

void KeyOption::set_key_string() {
    setText(KeyManager::keyType[keys[0]]);
    for(int i=1; i<keys.size(); i++) {
        setText(text() + "+" + KeyManager::keyType[keys[i]]);
    }
}

void KeyOption::reset() {
    keys = KeyManager::getKeys(window_name, key_name);;
    set_key_string();
    setChecked(false);
}

int KeyOption::getBeginIndex() {
    return 0;
}

int KeyOption::getDefaultIndex() {
    return index;
}

QString KeyOption::getName() {
    return window_name + key_name;
}

void KeyOption::restore() {
}
