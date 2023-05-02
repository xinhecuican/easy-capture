#include "KeyManager.h"
#include<QDebug>
#include "WindowManager.h"
#include "Helper/debug.h"
#include<QDomDocument>
#include<QFile>
#include<QKeyEvent>
#include "update.h"
#define WIN32_LEAN_AND_MEAN
#ifdef Q_OS_WIN
#include <Windows.h>
#endif
#include "MainFilter.h"

KeyManager::KeyManager() {

}


QHash<QString, KeyManager::window> KeyManager::allKey = QHash<QString, window>();
QList<int> KeyManager::availiableKey = QList<int>();
QSet<QString> KeyManager::usingWindowSet = QSet<QString>();
QList<KeyManager::listener_data> KeyManager::listeners = QList<listener_data>();
bool KeyManager::isWindowChange = false;
QList<KeyManager::GlobalKeyItem> KeyManager::globalKeys = QList<KeyManager::GlobalKeyItem>();
QList<QString> KeyManager::keySettings = {
    "CaptureWindow:leave;16777216",//Escape,可以通过Qt::Key_Escape查看键值
    "CaptureWindow:capture_rect;16777249,49",
    "CaptureWindow:capture_mosaic;16777249,50",
    "CaptureWindow:capture_cursor;16777249,51",
    "CaptureWindow:capture_pencil;16777249,52",
    "CaptureWindow:capture_highlighter;16777249,53",
    "CaptureWindow:capture_text;16777249,54",
    "CaptureWindow:capture_erase;16777249,55",
    "CaptureWindow:capture_undo;16777249,90",
    "CaptureWindow:capture_redo;16777249,88",
    "CaptureWindow:enter_capture;16777220",//enter
    "CaptureWindow:save2file;16777249,83",
    "CaptureWindow:save2clip;16777249,69",
    "CaptureWindow:capture_scrollrect;16777249",
    "PaintWindow:undo;16777249,90",
    "PaintWindow:redo;16777249,88",
    "PaintWindow:save;16777249,83",
    "PaintWindow:new_capture;16777249,78",
    "PaintWindow:delete_shape;16777223"
};

QList<QString> globalKeySetting = {
    "awake_capture:0;16777264?awake_capture",
    "fullscreen_capture:0;16777265?awake_capture",
    "capture_video_start:0;16777268?capture_video_start",
    "capture_video_pause:0;16777269?capture_video_pause",
    "capture_video_stop:0;16777270?capture_video_stop"
};

bool fastRegister[] = {true,
                       true,
                       false,
                       false,
                       false
                      };

void KeyManager::addKey(QString window_name, QString obj_name, QList<int> keys) {
    if(allKey.find(window_name) == allKey.end()) {
        window temp = window();
        temp.insert(obj_name, NULL, keys);
        allKey[window_name] = temp;
    } else {
        allKey[window_name].insert(obj_name, NULL, keys);
    }
}

void KeyManager::addFunc(QObject* receiver, QString window_name, QString obj_name, std::function<void(QObject*, bool)> const &f) {
    if(!usingWindowSet.contains(window_name)) {
        usingWindowSet.insert(window_name);
    }
    if(allKey.contains(window_name) && allKey[window_name].func.contains(obj_name)) {
        allKey[window_name].func[obj_name].func = f;
        allKey[window_name].func[obj_name].receiver = receiver;
    } else {
        qWarning("未找到对应按键\n位置：KeyManager::addFunc");
    }
}

void KeyManager::updateAll() {
    allKey = QHash<QString, window>();
    for(int i=0; i<keySettings.size(); i++) {
        int first_index = keySettings[i].indexOf(':');
        int second_index = keySettings[i].indexOf(';');
        QList<int> using_key = QList<int>();
        QString temp;
        for(int k=second_index+1; k<keySettings[i].size(); k++) {
            if(keySettings[i][k] == ',') {
                using_key.append(temp.toInt());
                temp.clear();
            } else {
                temp.append(keySettings[i][k]);
            }
        }
        using_key.append(temp.toInt());
        QString window_name = keySettings[i].mid(0, first_index);
        QString key_name = keySettings[i].mid(first_index+1, second_index-first_index-1);
        if(allKey.find(window_name) == allKey.end()) {
            addKey(window_name, key_name, using_key);
        } else if(allKey[window_name].func.find(key_name) == allKey[window_name].func.end()) {
            addKey(window_name, key_name, using_key);
        }
    }
    updateGlobalKey();
}

void KeyManager::updateGlobalKey() {
    for(int i=0; i<globalKeySetting.size(); i++) {
        int firstIndex = globalKeySetting[i].indexOf(':');
        int secondIndex = globalKeySetting[i].indexOf(';');
        int thirdIndex = globalKeySetting[i].indexOf('?');
        QString temp;
        QList<int> using_key = QList<int>();
        for(int k=secondIndex+1; k<thirdIndex; k++) {
            if(globalKeySetting[i][k] == ',') {
                using_key.append(temp.toInt());
                temp.clear();
            } else {
                temp.append(globalKeySetting[i][k]);
            }
        }
        using_key.append(temp.toInt());
        QString name = globalKeySetting[i].mid(0, firstIndex);
        int modKey = globalKeySetting[i].mid(firstIndex+1, secondIndex - firstIndex - 1).toInt();
        QString funcName = globalKeySetting[i].mid(thirdIndex+1);
        addGlobalKey(name, modKey, using_key.size() != 0 ? using_key[0] : 0, funcName);
        if(fastRegister[i])
            registerGlobalKey(name);
    }
}

void KeyManager::onKeyCountChange(bool is_enter, int key) {
    QString now_window = WindowManager::getNowWindow();
    window window_key = allKey[now_window];
    window_key.findAndRun(is_enter, key);
}

void KeyManager::keyEnter(int key) {
    if(availiableKey.contains(key) || isWindowChange) {
        return;
    }
    for(int i=0; i<listeners.size(); i++) {
        listeners[i].is_begin = true;
        listeners[i].listener->getKey(key);
    }
    onKeyCountChange(true, key);
}

void KeyManager::keyRelease(int key) {
    if(!availiableKey.contains(key) || isWindowChange) {
        return;
    }
    onKeyCountChange(false, key);
    if(availiableKey.size() == 0) {
        for(int i=0; i<listeners.size(); i++) {
            listeners[i].is_begin = false;
            listeners[i].listener->keyEnd();
        }
    }
}

bool KeyManager::isContainsWindow(QString window_name) {
    return usingWindowSet.contains(window_name);
}

void KeyManager::addKeyListener(IKeyListener* listener) {
    listener_data data;
    data.listener = listener;
    data.is_begin = false;
    if(availiableKey.size() != 0) {
        data.is_begin = true;
        for(int i=0; i<availiableKey.size(); i++) {
            listener->getKey(availiableKey[i]);
        }
    }
    listeners.append(data);
}

void KeyManager::removeKeyListener(IKeyListener *listener) {
    for(int i=0; i<listeners.size(); i++) {
        if(listeners[i].listener == listener) {
            listeners.removeAt(i);
            break;
        }
    }
}

void KeyManager::setKeys(QString window_name, QString key_name, QList<int> keys) {
    if(allKey.find(window_name) != allKey.end() &&
            allKey[window_name].func.find(key_name) != allKey[window_name].func.end()) {
        allKey[window_name].func[key_name].keys = keys;
    }
}

QList<int> KeyManager::getKeys(QString window_name, QString key_name) {
    if(allKey.find(window_name) != allKey.end() &&
            allKey[window_name].func.find(key_name) != allKey[window_name].func.end()) {
        return allKey[window_name].func[key_name].keys;
    }
    return QList<int>();
}

QList<QString> KeyManager::getWindowNames() {
    return allKey.keys();
}

QList<QString> KeyManager::getKeyNames(QString window_name) {
    if(allKey.find(window_name) != allKey.end()) {
        return allKey[window_name].func.keys();
    }
    return QList<QString>();
}

QList<QString> KeyManager::detectKeyConflict(QString window_name, QString key_name, QList<int> keys) {
    QList<QString> ans = QList<QString>();
    if(allKey.find(window_name) != allKey.end() &&
            allKey[window_name].func.find(key_name) != allKey[window_name].func.end()) {
        for(auto iter=allKey[window_name].func.begin(); iter!=allKey[window_name].func.end(); iter++) {
            if(iter.key() == key_name) {
                continue;
            }
            bool is_equal = true;
            for(int i=0; i<keys.size(); i++) {
                bool flag = false;
                for(int k=0; k<iter->keys.size(); k++) {
                    if(iter->keys[k] == keys[i]) {
                        flag = true;
                        break;
                    }
                }
                if(!flag) {
                    is_equal = false;
                    break;
                }
            }
            if(is_equal) {
                ans.append(iter.key());
            }
        }
    }
    return ans;
}

void KeyManager::save() {
    QFile file("Data/keys.xml");
    if(!file.open(QIODevice::ReadWrite | QIODevice::Truncate)) {
        qWarning("按键配置文件无法打开\n位置：KeyManager::save");
        return;
    }
    QDomDocument doc;
    QDomProcessingInstruction instruction; //添加处理命令
    instruction=doc.createProcessingInstruction("xml","version=\"1.0\" encoding=\"UTF-8\"");
    doc.appendChild(instruction);
    //添加根节点
    QDomElement root=doc.createElement("key_setting");
    root.setAttribute("version", Update::now_version.get_version());
    doc.appendChild(root);

    QHashIterator<QString, window> iter(allKey);
    while(iter.hasNext()) {
        iter.next();
        QDomElement element = doc.createElement("window");
        element.setAttribute("name", iter.key());
        QHashIterator<QString, node> iter2(iter.value().func);
        while(iter2.hasNext()) {
            iter2.next();
            QDomElement element2 = doc.createElement("bind_key");
            element2.setAttribute("name", iter2.key());
            for(int i=0; i<iter2.value().keys.size(); i++) {
                QDomElement element3 = doc.createElement("key");
                element3.setAttribute("id", iter2.value().keys[i]);
                element2.appendChild(element3);
            }
            element.appendChild(element2);
        }
        root.appendChild(element);
    }
    QDomElement element = doc.createElement("global_keys");
    for(GlobalKeyItem keyItem : globalKeys) {
        QDomElement element2 = doc.createElement("global_key");
        element2.setAttribute("name", keyItem.name);
        element2.setAttribute("mod_key", keyItem.modKey);
        element2.setAttribute("key", keyItem.key);
        element.appendChild(element2);
    }
    root.appendChild(element);

    QTextStream out_stream(&file);
    doc.save(out_stream,4); //缩进4格
    file.close();
}

void KeyManager::load() {
    QFile file("Data/keys.xml");
    if(!file.open(QIODevice::ReadOnly)) {
        updateAll();
    } else {
        QDomDocument doc;
        if(!doc.setContent(&file)) {
            file.close();
            return;
        }
        file.close();

        QDomElement root = doc.documentElement();
        if(root.tagName().compare("key_setting") == 0 && (root.attribute("version").compare(Update::now_version.get_version()) == 0)) {
            QDomNodeList childs = root.childNodes();
            bool globalKeySet = false;
            for(int i=0; i<childs.size(); i++) {
                QDomElement element = childs.at(i).toElement();
                if(element.tagName().compare("window") == 0) {
                    QString window_name = element.attribute("name");
                    QDomNodeList list = element.childNodes();
                    for(int k=0; k<list.size(); k++) {
                        QDomElement element2 = list.at(k).toElement();
                        QString obj_name = element2.attribute("name");
                        QDomNodeList key_list = element2.childNodes();
                        QList<int> keys = QList<int>();
                        for(int j=0; j<key_list.size(); j++) {
                            keys.append(key_list.at(j).toElement().attribute("id").toInt());
                        }
                        addKey(window_name, obj_name, keys);
                    }
                } else if(element.tagName().compare("global_keys") == 0) {
                    globalKeySet = true;
                    QDomNodeList list = element.childNodes();
                    for(int k=0; k<list.size(); k++) {
                        QDomElement element2 = list.at(k).toElement();
                        QString name = element2.attribute("name");
                        int modKey = element2.attribute("mod_key").toInt();
                        int key = element2.attribute("key").toInt();
                        addGlobalKey(name, modKey, key);
                    }
                }
            }
            if(!globalKeySet) {
                updateGlobalKey();
            }
        } else {
            updateAll();
        }
    }

}

void KeyManager::reset() {
    updateAll();
}

void KeyManager::onWindowChangeBegin(QString old_window, QString new_window) {
    if(old_window != "") {
        window old = allKey[old_window];
        for(auto iter=old.func.begin(); iter!=old.func.end(); iter++) { //无论有键按下还是松开都会让原来的键失效
            if(iter->isKeyEqual(availiableKey) && iter->func != NULL) {
                iter->func(iter->receiver, false);
                break;
            }
        }
    }
    availiableKey.clear();
    isWindowChange = true;
}

void KeyManager::onWindowChangeEnd() {
    isWindowChange = false;
}

void KeyManager::onWindowClose(QString windowName) {

}

#ifdef Q_OS_WIN
QList<ATOM> KeyManager::getGlobalKeyId() {
    QList<ATOM> ans;
    for(GlobalKeyItem keyItem : globalKeys) {
        ans.append(keyItem.keyId);
    }
    return ans;
}
#endif

int KeyManager::getGlobalKey(int index) {
    return globalKeys[index].key;
}

int KeyManager::getGlobalModKey(int index) {
    return globalKeys[index].modKey;
}

bool KeyManager::isGloablKeyRegistered(int index) {
    return globalKeys[index].registered;
}

QString KeyManager::getGlobalKeyName(int index) {
    return globalKeys[index].name;
}

QList<QString> KeyManager::getGlobalKeyName() {
    QList<QString> ans;
    for(int i=0; i<globalKeys.size(); i++) {
        ans.append(globalKeys[i].name);
    }
    return ans;
}

void KeyManager::addGlobalKey(QString name, int modKey, int key, QString funcName) {
    for(int i=0; i<globalKeys.size(); i++) {
        if(globalKeys[i].name == name) {
            globalKeys[i].modKey = modKey;
            globalKeys[i].key = key;
            if(funcName != "")
                globalKeys[i].funcName = funcName;
            return;
        }
    }
    GlobalKeyItem keyItem;
    keyItem.name = name;
    keyItem.modKey = modKey;
    keyItem.key = key;
    keyItem.registered = false;
    keyItem.funcName = funcName;
    keyItem.shortcut = new QxtGlobalShortcut(MainFilter::instance()); // MainFilter的生命周期是全局的
    globalKeys.append(keyItem);
}

void KeyManager::registerGlobalKey(QString name) {
    for(int i=0; i<globalKeys.size(); i++) {
        if(!globalKeys[i].registered && globalKeys[i].name == name) {
            QKeySequence keySequence(globalKeys[i].modKey + globalKeys[i].key);
            globalKeys[i].registered = globalKeys[i].shortcut->setShortcut(keySequence);
            if(globalKeys[i].registered){
                MainFilter::instance()->connect(globalKeys[i].shortcut, &QxtGlobalShortcut::activated, MainFilter::instance(), [=](){
                    MainFilter::instance()->onGlobalKeyTriggered(globalKeys[i].funcName != "" ? globalKeys[i].funcName : globalKeys[i].name);
                });
            }
            break;
        }
    }
}

void KeyManager::unRegisterGlobalKey(QString name) {
    for(int i=0; i<globalKeys.size(); i++) {
        if(globalKeys[i].name == name) {
            if(globalKeys[i].registered){
                globalKeys[i].shortcut->unsetShortcut();
                globalKeys[i].registered = false;
            }
            break;
        }
    }
}

quint32 KeyManager::nativeModKeyCode(Qt::Key keycode) {
#ifdef Q_OS_WIN
    switch(keycode) {
    case Qt::Key_Alt:
        return MOD_ALT;
    case Qt::Key_Control:
        return MOD_CONTROL;
    case Qt::Key_Shift:
        return MOD_SHIFT;
    case Qt::Key_Meta:
        return MOD_WIN;
    }
    return 0;
#endif
}

void KeyManager::unRegisterAll() {
    for(int i=0; i<globalKeys.size(); i++) {
        if(globalKeys[i].registered) {
            globalKeys[i].shortcut->unsetShortcut();
            globalKeys[i].registered = false;

        }
    }
}

void KeyManager::registerAll() {
    for(int i=0; i<globalKeys.size(); i++) {
        if(!globalKeys[i].registered && fastRegister[i]) {
            globalKeys[i].registered = globalKeys[i].shortcut->setShortcut(QKeySequence(globalKeys[i].modKey + globalKeys[i].key));
            if(globalKeys[i].registered){
                MainFilter::instance()->connect(globalKeys[i].shortcut, &QxtGlobalShortcut::activated, MainFilter::instance(), [=](){
                    MainFilter::instance()->onGlobalKeyTriggered(globalKeys[i].funcName != "" ? globalKeys[i].funcName : globalKeys[i].name);
                });
            }
        }
    }
}

quint32 KeyManager::nativeKeycode(Qt::Key keycode) {
#ifdef Q_OS_WIN
    if(keycode <= 0xFFFF) {//Try to obtain the key from it's "character"
        const SHORT vKey = VkKeyScanW(static_cast<WCHAR>(keycode));
        if(vKey > -1)
            return LOBYTE(vKey);
    }

    //find key from switch/case --> Only finds a very small subset of keys
    switch (keycode) {
    case Qt::Key_Escape:
        return VK_ESCAPE;
    case Qt::Key_Tab:
    case Qt::Key_Backtab:
        return VK_TAB;
    case Qt::Key_Backspace:
        return VK_BACK;
    case Qt::Key_Return:
    case Qt::Key_Enter:
        return VK_RETURN;
    case Qt::Key_Insert:
        return VK_INSERT;
    case Qt::Key_Delete:
        return VK_DELETE;
    case Qt::Key_Pause:
        return VK_PAUSE;
    case Qt::Key_Print:
        return VK_PRINT;
    case Qt::Key_Clear:
        return VK_CLEAR;
    case Qt::Key_Home:
        return VK_HOME;
    case Qt::Key_End:
        return VK_END;
    case Qt::Key_Left:
        return VK_LEFT;
    case Qt::Key_Up:
        return VK_UP;
    case Qt::Key_Right:
        return VK_RIGHT;
    case Qt::Key_Down:
        return VK_DOWN;
    case Qt::Key_PageUp:
        return VK_PRIOR;
    case Qt::Key_PageDown:
        return VK_NEXT;
    case Qt::Key_CapsLock:
        return VK_CAPITAL;
    case Qt::Key_NumLock:
        return VK_NUMLOCK;
    case Qt::Key_ScrollLock:
        return VK_SCROLL;

    case Qt::Key_F1:
        return VK_F1;
    case Qt::Key_F2:
        return VK_F2;
    case Qt::Key_F3:
        return VK_F3;
    case Qt::Key_F4:
        return VK_F4;
    case Qt::Key_F5:
        return VK_F5;
    case Qt::Key_F6:
        return VK_F6;
    case Qt::Key_F7:
        return VK_F7;
    case Qt::Key_F8:
        return VK_F8;
    case Qt::Key_F9:
        return VK_F9;
    case Qt::Key_F10:
        return VK_F10;
    case Qt::Key_F11:
        return VK_F11;
    case Qt::Key_F12:
        return VK_F12;
    case Qt::Key_F13:
        return VK_F13;
    case Qt::Key_F14:
        return VK_F14;
    case Qt::Key_F15:
        return VK_F15;
    case Qt::Key_F16:
        return VK_F16;
    case Qt::Key_F17:
        return VK_F17;
    case Qt::Key_F18:
        return VK_F18;
    case Qt::Key_F19:
        return VK_F19;
    case Qt::Key_F20:
        return VK_F20;
    case Qt::Key_F21:
        return VK_F21;
    case Qt::Key_F22:
        return VK_F22;
    case Qt::Key_F23:
        return VK_F23;
    case Qt::Key_F24:
        return VK_F24;

    case Qt::Key_Menu:
        return VK_APPS;
    case Qt::Key_Help:
        return VK_HELP;
    case Qt::Key_MediaNext:
        return VK_MEDIA_NEXT_TRACK;
    case Qt::Key_MediaPrevious:
        return VK_MEDIA_PREV_TRACK;
    case Qt::Key_MediaPlay:
        return VK_MEDIA_PLAY_PAUSE;
    case Qt::Key_MediaStop:
        return VK_MEDIA_STOP;
    case Qt::Key_VolumeDown:
        return VK_VOLUME_DOWN;
    case Qt::Key_VolumeUp:
        return VK_VOLUME_UP;
    case Qt::Key_VolumeMute:
        return VK_VOLUME_MUTE;
    case Qt::Key_Mode_switch:
        return VK_MODECHANGE;
    case Qt::Key_Select:
        return VK_SELECT;
    case Qt::Key_Printer:
        return VK_PRINT;
    case Qt::Key_Execute:
        return VK_EXECUTE;
    case Qt::Key_Sleep:
        return VK_SLEEP;
    case Qt::Key_Period:
        return VK_DECIMAL;
    case Qt::Key_Play:
        return VK_PLAY;
    case Qt::Key_Cancel:
        return VK_CANCEL;

    case Qt::Key_Forward:
        return VK_BROWSER_FORWARD;
    case Qt::Key_Refresh:
        return VK_BROWSER_REFRESH;
    case Qt::Key_Stop:
        return VK_BROWSER_STOP;
    case Qt::Key_Search:
        return VK_BROWSER_SEARCH;
    case Qt::Key_Favorites:
        return VK_BROWSER_FAVORITES;
    case Qt::Key_HomePage:
        return VK_BROWSER_HOME;

    case Qt::Key_LaunchMail:
        return VK_LAUNCH_MAIL;
    case Qt::Key_LaunchMedia:
        return VK_LAUNCH_MEDIA_SELECT;
    case Qt::Key_Launch0:
        return VK_LAUNCH_APP1;
    case Qt::Key_Launch1:
        return VK_LAUNCH_APP2;

    case Qt::Key_Massyo:
        return VK_OEM_FJ_MASSHOU;
    case Qt::Key_Touroku:
        return VK_OEM_FJ_TOUROKU;

    default:
        if(keycode <= 0xFFFF)
            return (byte)keycode;
        else {
            return 0;
        }
    }
#endif
}

QHash<int, QString> KeyManager::keyType = {

    {0x1000000, "Escape"},
    {0x1000001, "Tab"},
    {0x1000002, "Backtab"},
    {0x1000003, "Backspace"},
    {0x1000004, "Return"},
    {0x1000005, "Enter"},
    {0x1000006, "Insert"},
    {0x1000007, "Delete"},
    {0x1000008, "Pause"},
    {0x1000009, "Print"},
    {0x100000a, "SysReq"},
    {0x100000b, "Clear"},
    {0x1000010, "Home"},
    {0x1000011, "End"},
    {0x1000012, "Left"},
    {0x1000013, "Up"},
    {0x1000014, "Right"},
    {0x1000015, "Down"},
    {0x1000016, "PageUp"},
    {0x1000017, "PageDown"},
    {0x1000020, "Shift"},
    {0x1000021, "Ctrl"},
    {0x1000022, "Meta"},
    {0x1000023, "Alt"},
    {0x1001103, "AltGr"},
    {0x1000024, "CapsLock"},
    {0x1000025, "NumLock"},
    {0x1000026, "ScrollLock"},
    {0x1000030, "F1"},
    {0x1000031, "F2"},
    {0x1000032, "F3"},
    {0x1000033, "F4"},
    {0x1000034, "F5"},
    {0x1000035, "F6"},
    {0x1000036, "F7"},
    {0x1000037, "F8"},
    {0x1000038, "F9"},
    {0x1000039, "F10"},
    {0x100003a, "F11"},
    {0x100003b, "F12"},
    {0x100003c, "F13"},
    {0x100003d, "F14"},
    {0x100003e, "F15"},
    {0x100003f, "F16"},
    {0x1000053, "Super_L"},
    {0x1000054, "Super_R"},
    {0x1000055, "Menu"},
    {0x1000056, "Hyper_L"},
    {0x1000057, "Hyper_R"},
    {0x1000058, "Help"},
    {0x1000059, "Direction_L"},
    {0x1000060, "Direction_R"},
    {0x20, "Space"},
    {0x21, "Exclam"},
    {0x22, "QuoteDbl"},
    {0x23, "NumberSign"},
    {0x24, "Dollar"},
    {0x25, "Percent"},
    {0x26, "Ampersand"},
    {0x27, "Apostrophe"},
    {0x28, "ParenLeft"},
    {0x29, "ParenRight"},
    {0x2a, "Asterisk"},
    {0x2b, "Plus"},
    {0x2c, "Comma"},
    {0x2d, "Minus"},
    {0x2e, "Period"},
    {0x2f, "Slash"},
    {0x30, "Num0"},
    {0x31, "Num1"},
    {0x32, "Num2"},
    {0x33, "Num3"},
    {0x34, "Num4"},
    {0x35, "Num5"},
    {0x36, "Num6"},
    {0x37, "Num7"},
    {0x38, "Num8"},
    {0x39, "Num9"},
    {0x3a, "Colon"},
    {0x3b, "Semicolon"},
    {0x3c, "Less"},
    {0x3d, "Equal"},
    {0x3d, "Greater"},
    {0x3f, "Question"},
    {0x40, "At"},
    {0x41, "A"},
    {0x42, "B"},
    {0x43, "C"},
    {0x44, "D"},
    {0x45, "E"},
    {0x46, "F"},
    {0x47, "G"},
    {0x48, "H"},
    {0x49, "I"},
    {0x4a, "J"},
    {0x4b, "K"},
    {0x4c, "L"},
    {0x4d, "M"},
    {0x4e, "N"},
    {0x4f, "O"},
    {0x50, "P"},
    {0x51, "Q"},
    {0x52, "R"},
    {0x53, "S"},
    {0x54, "T"},
    {0x55, "U"},
    {0x56, "V"},
    {0x57, "W"},
    {0x58, "X"},
    {0x59, "Y"},
    {0x5a, "Z"},
    {0x5b, "["},
    {0x5c, "\\"},
    {0x5d, "]"},
    {0x5e, "^"},
    {0x5f, "_"},
    {0x60, "`"},
    {0x7b, "{"},
    {0x7c, "|"},
    {0x7d, "}"},
    {0x7e, "~"},
    {0xd7, "multiply"},
    {0xf7, "division"},
    {0xff, "ydiaeresis"},
    {0x1000061, "Back"},
    {0x1000062, "Forward"},
    {0x1000063, "Stop"},
    {0x1000064, "Refresh"},
    {0x1000070, "VolumeDown"},
    {0x1000071, "VolumeMute"},
    {0x1000072, "VolumeUP"},
    {0x2000000, "Shift"},
    {0x4000000, "Ctrl"},
    {0x8000000, "Alt"}
};
