#include "KeyManager.h"
#include<QDebug>
#include "WindowManager.h"
#include "../Helper/debug.h"
#include<QDomDocument>
#include<QFile>
#include<QKeyEvent>
#include "update.h"
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include "../MainFilter.h"
#include "hook.h"
#include <QKeyEvent>

KeyManager::KeyManager(){
    MainFilter::instance()->connect(xHook, &XGlobalHook::keyEvent, MainFilter::instance(), [=](PKBDLLHOOKSTRUCT pKeyHookStruct) {
        quint32 vkcode = pKeyHookStruct->vkCode;
        int key = qtKeyCode(vkcode);
        if(pKeyHookStruct->flags == 128){
            listenerKeySet.remove(vkcode);
            for(listener_data listener : listeners){
                listener.listener->keyEnd();
            }
        }
        else if(!listenerKeySet.contains(vkcode)){
            listenerKeySet.insert(vkcode);
            for(listener_data listener : listeners){
                listener.listener->getKey(key);
            }
        }
    });
}

void KeyManager::addKey(QString window_name, QString obj_name, QList<int> keys) {
    if(allKey.find(window_name) == allKey.end()) {
        KeyWindow temp = KeyWindow();
        temp.insert(obj_name, NULL, keys);
        allKey[window_name] = temp;
    } else {
        allKey[window_name].insert(obj_name, NULL, keys);
    }
}

void KeyManager::addFunc(QObject* receiver, QString window_name, QString obj_name, std::function<void(bool)> const &f) {
    if(allKey.contains(window_name) && allKey[window_name].func.contains(obj_name)) {
        allKey[window_name].func[obj_name].func = f;
    } else {
        qWarning("未找到对应按键\n位置：KeyManager::addFunc");
    }
}

void KeyManager::updateAll() {
    allKey = QHash<QString, KeyWindow>();
    loadDefault();
    for(auto iter=defaultKeys.begin(); iter!=defaultKeys.end(); iter++){
        allKey.insert(iter.key(), iter.value());
    }
    updateGlobalKey();
}

void KeyManager::updateGlobalKey() {
    globalKeys.clear();
    loadDefault();
    for(auto i : defaultGlobalKeys){
        globalKeys.append(i);
        if(i.init){
            registerGlobalKey(i.name);
        }
    }
}

void KeyManager::onKeyCountChange(QString name, bool is_enter, int key) {
    if(!allKey.contains(name)){
        return;
    }
    allKey[name].run(key, is_enter);
}

void KeyManager::keyEnter(QString name, QKeyEvent* event) {
    int key = event->key();
    if(!event->isAutoRepeat()){
        onKeyCountChange(name, true, key);
    }
}

void KeyManager::keyRelease(QString name, QKeyEvent* event) {
    int key = event->key();
    if(!event->isAutoRepeat()){
        onKeyCountChange(name, false, key);
    }
}

void KeyManager::addKeyListener(IKeyListener* listener) {
    listener_data data;
    data.listener = listener;
    data.is_begin = false;
    listeners.append(data);
    if(!xHook->isKeyHookRunning()){
        xHook->installKeyHook();
    }
}

void KeyManager::removeKeyListener(IKeyListener *listener) {
    for(int i=0; i<listeners.size(); i++) {
        if(listeners[i].listener == listener) {
            listeners.removeAt(i);
            break;
        }
    }
    if(listeners.size() == 0){
        if(xHook->isKeyHookRunning())
            xHook->uninstallKeyHook();
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

    QHashIterator<QString, KeyWindow> iter(allKey);
    while(iter.hasNext()) {
        iter.next();
        QDomElement element = doc.createElement("window");
        element.setAttribute("name", iter.key());
        QHashIterator<QString, LocalKeyItem> iter2(iter.value().func);
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
        element2.setAttribute("init", keyItem.init);
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
                        bool init = false;
                        if(element2.hasAttribute("init")){
                            init = element2.attribute("init").toInt();
                        }
                        else{
                            loadDefault();
                            for(GlobalKeyItem item : defaultGlobalKeys){
                                if(item.name == name){
                                    init = item.init;
                                    break;
                                }
                            }
                        }
                        addGlobalKey(name, modKey, key, init);
                        if(init){
                            registerGlobalKey(name);
                        }
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

void KeyManager::loadDefault() {
    if(isLoadDefault)
        return;
    QFile file(":/Data/keys.xml");
    if(!file.open(QIODevice::ReadOnly)) {
        qWarning() << "默认按键配置打开失败";
        return;
    } else {
        QDomDocument doc;
        if(!doc.setContent(&file)) {
            file.close();
            qWarning() << "默认按键配置打开失败";
            return;
        }
        file.close();
        isLoadDefault = true;
        defaultKeys.clear();
        defaultGlobalKeys.clear();
        QDomElement root = doc.documentElement();
        if(root.tagName().compare("key_setting") == 0) {
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
                        if(defaultKeys.find(window_name) == defaultKeys.end()) {
                            KeyWindow temp = KeyWindow();
                            temp.insert(obj_name, NULL, keys);
                            defaultKeys[window_name] = temp;
                        } else {
                            defaultKeys[window_name].insert(obj_name, NULL, keys);
                        }
                    }
                } else if(element.tagName().compare("global_keys") == 0) {
                    globalKeySet = true;
                    QDomNodeList list = element.childNodes();
                    for(int k=0; k<list.size(); k++) {
                        QDomElement element2 = list.at(k).toElement();
                        QString name = element2.attribute("name");
                        int modKey = element2.attribute("mod_key").toInt();
                        int key = element2.attribute("key").toInt();
                        bool init = element2.attribute("init").toInt();
                        for(int i=0; i<defaultGlobalKeys.size(); i++) {
                            if(defaultGlobalKeys[i].name == name) {
                                defaultGlobalKeys[i].modKey = modKey;
                                defaultGlobalKeys[i].key = key;
                                defaultGlobalKeys[i].init = init;
                                return;
                            }
                        }
                        GlobalKeyItem keyItem;
                        keyItem.name = name;
                        keyItem.modKey = modKey;
                        keyItem.key = key;
                        keyItem.registered = false;
                        keyItem.init = init;
                        keyItem.keyId = GlobalAddAtomA(keyItem.name.toStdString().c_str());
                        defaultGlobalKeys.append(keyItem);
                    }
                }
            }
        }
    }

}

void KeyManager::reset() {
    updateAll();
}

void KeyManager::onWindowChangeBegin(QString oldWindow) {
    auto window = allKey.find(oldWindow);
    if(window != allKey.end()){
        window.value().currentKeys.clear();
    }
}

void KeyManager::onWindowChangeEnd(QString newWindow) {
}

void KeyManager::onWindowClose(QString windowName) {

}

QList<ATOM> KeyManager::getGlobalKeyId() {
    QList<ATOM> ans;
    for(GlobalKeyItem keyItem : globalKeys) {
        ans.append(keyItem.keyId);
    }
    return ans;
}

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

void KeyManager::addGlobalKey(QString name, int modKey, int key, bool init) {
    for(int i=0; i<globalKeys.size(); i++) {
        if(globalKeys[i].name == name) {
            globalKeys[i].modKey = modKey;
            globalKeys[i].key = key;
            globalKeys[i].init = init;
            return;
        }
    }
    GlobalKeyItem keyItem;
    keyItem.name = name;
    keyItem.modKey = modKey;
    keyItem.key = key;
    keyItem.registered = false;
    keyItem.init = init;
    keyItem.keyId = GlobalAddAtomA(keyItem.name.toStdString().c_str());
    globalKeys.append(keyItem);
}

void KeyManager::registerGlobalKey(QString name) {
    for(int i=0; i<globalKeys.size(); i++) {
        if(globalKeys[i].name == name) {
            if(globalKeys[i].registered) {
                UnregisterHotKey((HWND)MainFilter::instance()->winId(), globalKeys[i].keyId);
            }
            globalKeys[i].registered = RegisterHotKey((HWND)MainFilter::instance()->winId(), globalKeys[i].keyId, nativeModKeyCode((Qt::Key)globalKeys[i].modKey), nativeKeycode((Qt::Key)globalKeys[i].key));
            break;
        }
    }
}

void KeyManager::unRegisterGlobalKey(QString name) {
    for(int i=0; i<globalKeys.size(); i++) {
        if(globalKeys[i].name == name) {
            UnregisterHotKey((HWND)MainFilter::instance()->winId(), globalKeys[i].keyId);
            globalKeys[i].registered = false;
            break;
        }
    }
}

void KeyManager::updateKey(QString windowName, QString keyName, QList<int> keys){
    setKeys(windowName, keyName, keys);
}

void KeyManager::updateGlobalKey(QString name, int key, int modKey){
    for(int i=0; i<globalKeys.size(); i++){
        if(globalKeys[i].name == name){
            globalKeys[i].key = key;
            globalKeys[i].modKey = modKey;
            if(globalKeys[i].registered){
                UnregisterHotKey((HWND)MainFilter::instance()->winId(), globalKeys[i].keyId);
                globalKeys[i].registered = false;
            }
            if(globalKeys[i].init){
                globalKeys[i].registered = RegisterHotKey((HWND)MainFilter::instance()->winId(),
                                                          globalKeys[i].keyId,
                                                          nativeModKeyCode((Qt::Key)globalKeys[i].modKey),
                                                          nativeKeycode((Qt::Key)globalKeys[i].key));
            }
            save();
            break;
        }
    }
}

quint32 KeyManager::nativeModKeyCode(Qt::Key keycode) {
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
}

void KeyManager::unRegisterAll() {
    for(int i=0; i<globalKeys.size(); i++) {
        if(globalKeys[i].registered) {
            UnregisterHotKey((HWND)MainFilter::instance()->winId(), globalKeys[i].keyId);
            globalKeys[i].registered = false;

        }
    }
}

void KeyManager::registerAll() {
    for(int i=0; i<globalKeys.size(); i++) {
        if(!globalKeys[i].registered && globalKeys[i].init) {
            globalKeys[i].registered = RegisterHotKey((HWND)MainFilter::instance()->winId(), globalKeys[i].keyId, nativeModKeyCode((Qt::Key)globalKeys[i].modKey), nativeKeycode((Qt::Key)globalKeys[i].key));
        }
    }
}

void KeyManager::clearKeys(QString name){
    auto window = allKey.find(name);
    if(window != allKey.end()){
        window->currentKeys.clear();
    }
}

bool KeyManager::testGlobalKey(int index){
    if(globalKeys[index].init) {
        return globalKeys[index].registered;
    }
    else {
        return testGlobalKey(globalKeys[index].key, globalKeys[index].modKey);
    }
}

bool KeyManager::testGlobalKey(int key, int modKey){
    ATOM testId = GlobalAddAtomA("easycapture_test");
    bool result = RegisterHotKey((HWND)MainFilter::instance()->winId(), testId, KeyManager::instance()->nativeModKeyCode((Qt::Key)modKey), KeyManager::instance()->nativeKeycode((Qt::Key)key));
    UnregisterHotKey((HWND)MainFilter::instance()->winId(), testId);
    GlobalDeleteAtom(testId);
    return result;
}

quint32 KeyManager::nativeKeycode(Qt::Key keycode) {
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
}

static const uint KeyTbl[] = { // Keyboard mapping table
    // Dec |  Hex | Windows Virtual key
    Qt::Key_unknown,    //   0   0x00
    Qt::Key_unknown,    //   1   0x01   VK_LBUTTON          | Left mouse button
    Qt::Key_unknown,    //   2   0x02   VK_RBUTTON          | Right mouse button
    Qt::Key_Cancel,     //   3   0x03   VK_CANCEL           | Control-Break processing
    Qt::Key_unknown,    //   4   0x04   VK_MBUTTON          | Middle mouse button
    Qt::Key_unknown,    //   5   0x05   VK_XBUTTON1         | X1 mouse button
    Qt::Key_unknown,    //   6   0x06   VK_XBUTTON2         | X2 mouse button
    Qt::Key_unknown,    //   7   0x07   -- unassigned --
    Qt::Key_Backspace,  //   8   0x08   VK_BACK             | BackSpace key
    Qt::Key_Tab,        //   9   0x09   VK_TAB              | Tab key
    Qt::Key_unknown,    //  10   0x0A   -- reserved --
    Qt::Key_unknown,    //  11   0x0B   -- reserved --
    Qt::Key_Clear,      //  12   0x0C   VK_CLEAR            | Clear key
    Qt::Key_Return,     //  13   0x0D   VK_RETURN           | Enter key
    Qt::Key_unknown,    //  14   0x0E   -- unassigned --
    Qt::Key_unknown,    //  15   0x0F   -- unassigned --
    Qt::Key_Shift,      //  16   0x10   VK_SHIFT            | Shift key
    Qt::Key_Control,    //  17   0x11   VK_CONTROL          | Ctrl key
    Qt::Key_Alt,        //  18   0x12   VK_MENU             | Alt key
    Qt::Key_Pause,      //  19   0x13   VK_PAUSE            | Pause key
    Qt::Key_CapsLock,   //  20   0x14   VK_CAPITAL          | Caps-Lock
    Qt::Key_unknown,    //  21   0x15   VK_KANA / VK_HANGUL | IME Kana or Hangul mode
    Qt::Key_unknown,    //  22   0x16   -- unassigned --
    Qt::Key_unknown,    //  23   0x17   VK_JUNJA            | IME Junja mode
    Qt::Key_unknown,    //  24   0x18   VK_FINAL            | IME final mode
    Qt::Key_unknown,    //  25   0x19   VK_HANJA / VK_KANJI | IME Hanja or Kanji mode
    Qt::Key_unknown,    //  26   0x1A   -- unassigned --
    Qt::Key_Escape,     //  27   0x1B   VK_ESCAPE           | Esc key
    Qt::Key_unknown,    //  28   0x1C   VK_CONVERT          | IME convert
    Qt::Key_unknown,    //  29   0x1D   VK_NONCONVERT       | IME non-convert
    Qt::Key_unknown,    //  30   0x1E   VK_ACCEPT           | IME accept
    Qt::Key_Mode_switch,//  31   0x1F   VK_MODECHANGE       | IME mode change request
    Qt::Key_Space,      //  32   0x20   VK_SPACE            | Spacebar
    Qt::Key_PageUp,     //  33   0x21   VK_PRIOR            | Page Up key
    Qt::Key_PageDown,   //  34   0x22   VK_NEXT             | Page Down key
    Qt::Key_End,        //  35   0x23   VK_END              | End key
    Qt::Key_Home,       //  36   0x24   VK_HOME             | Home key
    Qt::Key_Left,       //  37   0x25   VK_LEFT             | Left arrow key
    Qt::Key_Up,         //  38   0x26   VK_UP               | Up arrow key
    Qt::Key_Right,      //  39   0x27   VK_RIGHT            | Right arrow key
    Qt::Key_Down,       //  40   0x28   VK_DOWN             | Down arrow key
    Qt::Key_Select,     //  41   0x29   VK_SELECT           | Select key
    Qt::Key_Printer,    //  42   0x2A   VK_PRINT            | Print key
    Qt::Key_Execute,    //  43   0x2B   VK_EXECUTE          | Execute key
    Qt::Key_Print,      //  44   0x2C   VK_SNAPSHOT         | Print Screen key
    Qt::Key_Insert,     //  45   0x2D   VK_INSERT           | Ins key
    Qt::Key_Delete,     //  46   0x2E   VK_DELETE           | Del key
    Qt::Key_Help,       //  47   0x2F   VK_HELP             | Help key
    0,                  //  48   0x30   (VK_0)              | 0 key
    0,                  //  49   0x31   (VK_1)              | 1 key
    0,                  //  50   0x32   (VK_2)              | 2 key
    0,                  //  51   0x33   (VK_3)              | 3 key
    0,                  //  52   0x34   (VK_4)              | 4 key
    0,                  //  53   0x35   (VK_5)              | 5 key
    0,                  //  54   0x36   (VK_6)              | 6 key
    0,                  //  55   0x37   (VK_7)              | 7 key
    0,                  //  56   0x38   (VK_8)              | 8 key
    0,                  //  57   0x39   (VK_9)              | 9 key
    Qt::Key_unknown,    //  58   0x3A   -- unassigned --
    Qt::Key_unknown,    //  59   0x3B   -- unassigned --
    Qt::Key_unknown,    //  60   0x3C   -- unassigned --
    Qt::Key_unknown,    //  61   0x3D   -- unassigned --
    Qt::Key_unknown,    //  62   0x3E   -- unassigned --
    Qt::Key_unknown,    //  63   0x3F   -- unassigned --
    Qt::Key_unknown,    //  64   0x40   -- unassigned --
    0,                  //  65   0x41   (VK_A)              | A key
    0,                  //  66   0x42   (VK_B)              | B key
    0,                  //  67   0x43   (VK_C)              | C key
    0,                  //  68   0x44   (VK_D)              | D key
    0,                  //  69   0x45   (VK_E)              | E key
    0,                  //  70   0x46   (VK_F)              | F key
    0,                  //  71   0x47   (VK_G)              | G key
    0,                  //  72   0x48   (VK_H)              | H key
    0,                  //  73   0x49   (VK_I)              | I key
    0,                  //  74   0x4A   (VK_J)              | J key
    0,                  //  75   0x4B   (VK_K)              | K key
    0,                  //  76   0x4C   (VK_L)              | L key
    0,                  //  77   0x4D   (VK_M)              | M key
    0,                  //  78   0x4E   (VK_N)              | N key
    0,                  //  79   0x4F   (VK_O)              | O key
    0,                  //  80   0x50   (VK_P)              | P key
    0,                  //  81   0x51   (VK_Q)              | Q key
    0,                  //  82   0x52   (VK_R)              | R key
    0,                  //  83   0x53   (VK_S)              | S key
    0,                  //  84   0x54   (VK_T)              | T key
    0,                  //  85   0x55   (VK_U)              | U key
    0,                  //  86   0x56   (VK_V)              | V key
    0,                  //  87   0x57   (VK_W)              | W key
    0,                  //  88   0x58   (VK_X)              | X key
    0,                  //  89   0x59   (VK_Y)              | Y key
    0,                  //  90   0x5A   (VK_Z)              | Z key
    Qt::Key_Meta,       //  91   0x5B   VK_LWIN             | Left Windows  - MS Natural kbd
    Qt::Key_Meta,       //  92   0x5C   VK_RWIN             | Right Windows - MS Natural kbd
    Qt::Key_Menu,       //  93   0x5D   VK_APPS             | Application key-MS Natural kbd
    Qt::Key_unknown,    //  94   0x5E   -- reserved --
    Qt::Key_Sleep,      //  95   0x5F   VK_SLEEP
    Qt::Key_0,          //  96   0x60   VK_NUMPAD0          | Numeric keypad 0 key
    Qt::Key_1,          //  97   0x61   VK_NUMPAD1          | Numeric keypad 1 key
    Qt::Key_2,          //  98   0x62   VK_NUMPAD2          | Numeric keypad 2 key
    Qt::Key_3,          //  99   0x63   VK_NUMPAD3          | Numeric keypad 3 key
    Qt::Key_4,          // 100   0x64   VK_NUMPAD4          | Numeric keypad 4 key
    Qt::Key_5,          // 101   0x65   VK_NUMPAD5          | Numeric keypad 5 key
    Qt::Key_6,          // 102   0x66   VK_NUMPAD6          | Numeric keypad 6 key
    Qt::Key_7,          // 103   0x67   VK_NUMPAD7          | Numeric keypad 7 key
    Qt::Key_8,          // 104   0x68   VK_NUMPAD8          | Numeric keypad 8 key
    Qt::Key_9,          // 105   0x69   VK_NUMPAD9          | Numeric keypad 9 key
    Qt::Key_Asterisk,   // 106   0x6A   VK_MULTIPLY         | Multiply key
    Qt::Key_Plus,       // 107   0x6B   VK_ADD              | Add key
    Qt::Key_Comma,      // 108   0x6C   VK_SEPARATOR        | Separator key
    Qt::Key_Minus,      // 109   0x6D   VK_SUBTRACT         | Subtract key
    Qt::Key_Period,     // 110   0x6E   VK_DECIMAL          | Decimal key
    Qt::Key_Slash,      // 111   0x6F   VK_DIVIDE           | Divide key
    Qt::Key_F1,         // 112   0x70   VK_F1               | F1 key
    Qt::Key_F2,         // 113   0x71   VK_F2               | F2 key
    Qt::Key_F3,         // 114   0x72   VK_F3               | F3 key
    Qt::Key_F4,         // 115   0x73   VK_F4               | F4 key
    Qt::Key_F5,         // 116   0x74   VK_F5               | F5 key
    Qt::Key_F6,         // 117   0x75   VK_F6               | F6 key
    Qt::Key_F7,         // 118   0x76   VK_F7               | F7 key
    Qt::Key_F8,         // 119   0x77   VK_F8               | F8 key
    Qt::Key_F9,         // 120   0x78   VK_F9               | F9 key
    Qt::Key_F10,        // 121   0x79   VK_F10              | F10 key
    Qt::Key_F11,        // 122   0x7A   VK_F11              | F11 key
    Qt::Key_F12,        // 123   0x7B   VK_F12              | F12 key
    Qt::Key_F13,        // 124   0x7C   VK_F13              | F13 key
    Qt::Key_F14,        // 125   0x7D   VK_F14              | F14 key
    Qt::Key_F15,        // 126   0x7E   VK_F15              | F15 key
    Qt::Key_F16,        // 127   0x7F   VK_F16              | F16 key
    Qt::Key_F17,        // 128   0x80   VK_F17              | F17 key
    Qt::Key_F18,        // 129   0x81   VK_F18              | F18 key
    Qt::Key_F19,        // 130   0x82   VK_F19              | F19 key
    Qt::Key_F20,        // 131   0x83   VK_F20              | F20 key
    Qt::Key_F21,        // 132   0x84   VK_F21              | F21 key
    Qt::Key_F22,        // 133   0x85   VK_F22              | F22 key
    Qt::Key_F23,        // 134   0x86   VK_F23              | F23 key
    Qt::Key_F24,        // 135   0x87   VK_F24              | F24 key
    Qt::Key_unknown,    // 136   0x88   -- unassigned --
    Qt::Key_unknown,    // 137   0x89   -- unassigned --
    Qt::Key_unknown,    // 138   0x8A   -- unassigned --
    Qt::Key_unknown,    // 139   0x8B   -- unassigned --
    Qt::Key_unknown,    // 140   0x8C   -- unassigned --
    Qt::Key_unknown,    // 141   0x8D   -- unassigned --
    Qt::Key_unknown,    // 142   0x8E   -- unassigned --
    Qt::Key_unknown,    // 143   0x8F   -- unassigned --
    Qt::Key_NumLock,    // 144   0x90   VK_NUMLOCK          | Num Lock key
    Qt::Key_ScrollLock, // 145   0x91   VK_SCROLL           | Scroll Lock key
    // Fujitsu/OASYS kbd --------------------
    0, //Qt::Key_Jisho, // 146   0x92   VK_OEM_FJ_JISHO     | 'Dictionary' key /
    //              VK_OEM_NEC_EQUAL  = key on numpad on NEC PC-9800 kbd
    Qt::Key_Massyo,     // 147   0x93   VK_OEM_FJ_MASSHOU   | 'Unregister word' key
    Qt::Key_Touroku,    // 148   0x94   VK_OEM_FJ_TOUROKU   | 'Register word' key
    0, //Qt::Key_Oyayubi_Left,//149   0x95  VK_OEM_FJ_LOYA  | 'Left OYAYUBI' key
    0, //Qt::Key_Oyayubi_Right,//150  0x96  VK_OEM_FJ_ROYA  | 'Right OYAYUBI' key
    Qt::Key_unknown,    // 151   0x97   -- unassigned --
    Qt::Key_unknown,    // 152   0x98   -- unassigned --
    Qt::Key_unknown,    // 153   0x99   -- unassigned --
    Qt::Key_unknown,    // 154   0x9A   -- unassigned --
    Qt::Key_unknown,    // 155   0x9B   -- unassigned --
    Qt::Key_unknown,    // 156   0x9C   -- unassigned --
    Qt::Key_unknown,    // 157   0x9D   -- unassigned --
    Qt::Key_unknown,    // 158   0x9E   -- unassigned --
    Qt::Key_unknown,    // 159   0x9F   -- unassigned --
    Qt::Key_Shift,      // 160   0xA0   VK_LSHIFT           | Left Shift key
    Qt::Key_Shift,      // 161   0xA1   VK_RSHIFT           | Right Shift key
    Qt::Key_Control,    // 162   0xA2   VK_LCONTROL         | Left Ctrl key
    Qt::Key_Control,    // 163   0xA3   VK_RCONTROL         | Right Ctrl key
    Qt::Key_Alt,        // 164   0xA4   VK_LMENU            | Left Menu key
    Qt::Key_Alt,        // 165   0xA5   VK_RMENU            | Right Menu key
    Qt::Key_Back,       // 166   0xA6   VK_BROWSER_BACK     | Browser Back key
    Qt::Key_Forward,    // 167   0xA7   VK_BROWSER_FORWARD  | Browser Forward key
    Qt::Key_Refresh,    // 168   0xA8   VK_BROWSER_REFRESH  | Browser Refresh key
    Qt::Key_Stop,       // 169   0xA9   VK_BROWSER_STOP     | Browser Stop key
    Qt::Key_Search,     // 170   0xAA   VK_BROWSER_SEARCH   | Browser Search key
    Qt::Key_Favorites,  // 171   0xAB   VK_BROWSER_FAVORITES| Browser Favorites key
    Qt::Key_HomePage,   // 172   0xAC   VK_BROWSER_HOME     | Browser Start and Home key
    Qt::Key_VolumeMute, // 173   0xAD   VK_VOLUME_MUTE      | Volume Mute key
    Qt::Key_VolumeDown, // 174   0xAE   VK_VOLUME_DOWN      | Volume Down key
    Qt::Key_VolumeUp,   // 175   0xAF   VK_VOLUME_UP        | Volume Up key
    Qt::Key_MediaNext,  // 176   0xB0   VK_MEDIA_NEXT_TRACK | Next Track key
    Qt::Key_MediaPrevious, //177 0xB1   VK_MEDIA_PREV_TRACK | Previous Track key
    Qt::Key_MediaStop,  // 178   0xB2   VK_MEDIA_STOP       | Stop Media key
    Qt::Key_MediaPlay,  // 179   0xB3   VK_MEDIA_PLAY_PAUSE | Play/Pause Media key
    Qt::Key_LaunchMail, // 180   0xB4   VK_LAUNCH_MAIL      | Start Mail key
    Qt::Key_LaunchMedia,// 181   0xB5   VK_LAUNCH_MEDIA_SELECT Select Media key
    Qt::Key_Launch0,    // 182   0xB6   VK_LAUNCH_APP1      | Start Application 1 key
    Qt::Key_Launch1,    // 183   0xB7   VK_LAUNCH_APP2      | Start Application 2 key
    Qt::Key_unknown,    // 184   0xB8   -- reserved --
    Qt::Key_unknown,    // 185   0xB9   -- reserved --
    0,                  // 186   0xBA   VK_OEM_1            | ';:' for US
    0,                  // 187   0xBB   VK_OEM_PLUS         | '+' any country
    0,                  // 188   0xBC   VK_OEM_COMMA        | ',' any country
    0,                  // 189   0xBD   VK_OEM_MINUS        | '-' any country
    0,                  // 190   0xBE   VK_OEM_PERIOD       | '.' any country
    0,                  // 191   0xBF   VK_OEM_2            | '/?' for US
    0,                  // 192   0xC0   VK_OEM_3            | '`~' for US
    Qt::Key_unknown,    // 193   0xC1   -- reserved --
    Qt::Key_unknown,    // 194   0xC2   -- reserved --
    Qt::Key_unknown,    // 195   0xC3   -- reserved --
    Qt::Key_unknown,    // 196   0xC4   -- reserved --
    Qt::Key_unknown,    // 197   0xC5   -- reserved --
    Qt::Key_unknown,    // 198   0xC6   -- reserved --
    Qt::Key_unknown,    // 199   0xC7   -- reserved --
    Qt::Key_unknown,    // 200   0xC8   -- reserved --
    Qt::Key_unknown,    // 201   0xC9   -- reserved --
    Qt::Key_unknown,    // 202   0xCA   -- reserved --
    Qt::Key_unknown,    // 203   0xCB   -- reserved --
    Qt::Key_unknown,    // 204   0xCC   -- reserved --
    Qt::Key_unknown,    // 205   0xCD   -- reserved --
    Qt::Key_unknown,    // 206   0xCE   -- reserved --
    Qt::Key_unknown,    // 207   0xCF   -- reserved --
    Qt::Key_unknown,    // 208   0xD0   -- reserved --
    Qt::Key_unknown,    // 209   0xD1   -- reserved --
    Qt::Key_unknown,    // 210   0xD2   -- reserved --
    Qt::Key_unknown,    // 211   0xD3   -- reserved --
    Qt::Key_unknown,    // 212   0xD4   -- reserved --
    Qt::Key_unknown,    // 213   0xD5   -- reserved --
    Qt::Key_unknown,    // 214   0xD6   -- reserved --
    Qt::Key_unknown,    // 215   0xD7   -- reserved --
    Qt::Key_unknown,    // 216   0xD8   -- unassigned --
    Qt::Key_unknown,    // 217   0xD9   -- unassigned --
    Qt::Key_unknown,    // 218   0xDA   -- unassigned --
    0,                  // 219   0xDB   VK_OEM_4            | '[{' for US
    0,                  // 220   0xDC   VK_OEM_5            | '\|' for US
    0,                  // 221   0xDD   VK_OEM_6            | ']}' for US
    0,                  // 222   0xDE   VK_OEM_7            | ''"' for US
    0,                  // 223   0xDF   VK_OEM_8
    Qt::Key_unknown,    // 224   0xE0   -- reserved --
    Qt::Key_unknown,    // 225   0xE1   VK_OEM_AX           | 'AX' key on Japanese AX kbd
    Qt::Key_unknown,    // 226   0xE2   VK_OEM_102          | "<>" or "\|" on RT 102-key kbd
    Qt::Key_unknown,    // 227   0xE3   VK_ICO_HELP         | Help key on ICO
    Qt::Key_unknown,    // 228   0xE4   VK_ICO_00           | 00 key on ICO
    Qt::Key_unknown,    // 229   0xE5   VK_PROCESSKEY       | IME Process key
    Qt::Key_unknown,    // 230   0xE6   VK_ICO_CLEAR        |
    Qt::Key_unknown,    // 231   0xE7   VK_PACKET           | Unicode char as keystrokes
    Qt::Key_unknown,    // 232   0xE8   -- unassigned --
    // Nokia/Ericsson definitions ---------------
    Qt::Key_unknown,    // 233   0xE9   VK_OEM_RESET
    Qt::Key_unknown,    // 234   0xEA   VK_OEM_JUMP
    Qt::Key_unknown,    // 235   0xEB   VK_OEM_PA1
    Qt::Key_unknown,    // 236   0xEC   VK_OEM_PA2
    Qt::Key_unknown,    // 237   0xED   VK_OEM_PA3
    Qt::Key_unknown,    // 238   0xEE   VK_OEM_WSCTRL
    Qt::Key_unknown,    // 239   0xEF   VK_OEM_CUSEL
    Qt::Key_unknown,    // 240   0xF0   VK_OEM_ATTN
    Qt::Key_unknown,    // 241   0xF1   VK_OEM_FINISH
    Qt::Key_unknown,    // 242   0xF2   VK_OEM_COPY
    Qt::Key_unknown,    // 243   0xF3   VK_OEM_AUTO
    Qt::Key_unknown,    // 244   0xF4   VK_OEM_ENLW
    Qt::Key_unknown,    // 245   0xF5   VK_OEM_BACKTAB
    Qt::Key_unknown,    // 246   0xF6   VK_ATTN             | Attn key
    Qt::Key_unknown,    // 247   0xF7   VK_CRSEL            | CrSel key
    Qt::Key_unknown,    // 248   0xF8   VK_EXSEL            | ExSel key
    Qt::Key_unknown,    // 249   0xF9   VK_EREOF            | Erase EOF key
    Qt::Key_Play,       // 250   0xFA   VK_PLAY             | Play key
    Qt::Key_Zoom,       // 251   0xFB   VK_ZOOM             | Zoom key
    Qt::Key_unknown,    // 252   0xFC   VK_NONAME           | Reserved
    Qt::Key_unknown,    // 253   0xFD   VK_PA1              | PA1 key
    Qt::Key_Clear,      // 254   0xFE   VK_OEM_CLEAR        | Clear key
    0
};

int KeyManager::qtKeyCode(quint32 keycode){
    if(keycode < 0 || keycode > 255){
        return Qt::Key_unknown;
    }
    if(KeyTbl[keycode] == 0){
        return keycode;
    }
    return KeyTbl[keycode];
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
    {0x1000072, "VolumeUP"}

};
