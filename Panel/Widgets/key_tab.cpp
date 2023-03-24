#include "key_tab.h"
#include "Manager/KeyManager.h"

Key_tab::Key_tab() {

}

Key_tab::Key_tab(int index, QString window_name, QString key_name, QWidget* parent) : QPushButton(parent) {
    this->index = index;
    this->window_name = window_name;
    this->key_name = key_name;
    keys = KeyManager::getKeys(window_name, key_name);
    set_key_string();
    setCheckable(true);
    connect(this, &QPushButton::clicked, this, [=]() {
        setText("");
        temp_keys = QList<int>();
        KeyManager::addKeyListener(this);
    });
    originKeys = keys;
    dirty = false;
}

void Key_tab::getKey(int key) {
    temp_keys.append(key);
    if(text() == "") {
        setText(text() + KeyManager::keyType[key]);
    } else {
        setText(text() + "+" + KeyManager::keyType[key]);
    }
}

void Key_tab::keyEnd() {
    KeyManager::remvoeKeyListener(this);
    QList<QString> key_names = KeyManager::detectKeyConflict(window_name, key_name, temp_keys);
    if(key_names.size() != 0) {
        for(int i=0; i<key_names.size(); i++) {
            key_names[i] = window_name + key_names[i];
        }

        emit key_conflict(key_names);
    } else if(temp_keys.size() != 0) {
        setChecked(false);
        KeyManager::setKeys(window_name, key_name, temp_keys);
        keys = temp_keys;
        dirty = true;
    } else {
        set_key_string();
        setChecked(false);
    }
}

void Key_tab::set_key_string() {
    setText(KeyManager::keyType[keys[0]]);
    for(int i=1; i<keys.size(); i++) {
        setText(text() + "+" + KeyManager::keyType[keys[i]]);
    }
}

void Key_tab::reset() {

}

int Key_tab::getBeginIndex() {
    return 0;
}

int Key_tab::getDefaultIndex() {
    return index;
}

QString Key_tab::getName() {
    return window_name + key_name;
}

void Key_tab::restore() {
    if(dirty) {
        KeyManager::setKeys(window_name, key_name, originKeys);
    }
}
