#ifndef KEY_MANAGER_H
#define KEY_MANAGER_H
#include <QString>
#include<functional>
#include<QList>
#include<cstdarg>
#include "Helper/debug.h"
#include "Helper/EnumReflect.h"
#include "IKeyListener.h"
#include "QAbstractNativeEventFilter"
#include <windows.h>

class KeyManager {
public:

    KeyManager();
    static void addFunc(QObject* receiver, QString window_name, QString obj_name, std::function<void(QObject*, bool)> const &f);
    static void addKey(QString window_name, QString obj_name, QList<int> keys);
    static void onKeyCountChange(bool is_enter, int key);//按键按下或松开时的处理函数
    static void keyEnter(int key);
    static void keyRelease(int key);
    static bool isContainsWindow(QString window_name);
    static void addKeyListener(IKeyListener* listener);
    static void removeKeyListener(IKeyListener* listener);
    static void save();
    static void load();
    static void reset();
    static void updateAll();
    static void updateGlobalKey();
    static void onWindowChangeBegin(QString old_window, QString new_window);
    static void onWindowChangeEnd();
    static void onWindowClose(QString windowName);
    static QList<int> getKeys(QString window_name, QString key_name);
    static void setKeys(QString window_name, QString key_name, QList<int> keys);
    static QList<QString> getWindowNames();
    static QList<QString> getKeyNames(QString window_name);
    static QList<QString> detectKeyConflict(QString window_name, QString key_name, QList<int> keys);

    static quint32 nativeKeycode(Qt::Key keycode);
    static quint32 nativeModKeyCode(Qt::Key keycode);
    static QList<ATOM> getGlobalKeyId();
    static QList<QString> getGlobalKeyName();
    static QString getGlobalKeyName(int index);
    static void addGlobalKey(QString name, int modKey, int key);
    static void registerGlobalKey(QString name);
    static void unRegisterGlobalKey(QString name);
    static int getGlobalModKey(int index);
    static int getGlobalKey(int index);
    static bool isGloablKeyRegistered(int index);
    static void unRegisterAll();
    static void registerAll();
private:
    struct GlobalKeyItem {
        bool registered;
        ATOM keyId;
        int modKey;
        int key;
        QString name;
    };
    struct node {
        std::function<void(QObject*, bool)> func;
        QObject* receiver;
        QList<int> keys;
        node() {
            func = NULL;
            keys = QList<int>();
            receiver = NULL;
        }
        node(std::function<void(QObject*, bool)> const &f, QList<int> &key) {
            func = f;
            keys = key;
        }

        bool isKeyEqual(QList<int> &key) {
            if(key.size() != keys.size())
                return false;
            for(int i=0; i<keys.size(); i++) {
                bool equal = false;
                for(int k=0; k<key.size(); k++) {
                    if(keys[i] == key[k]) {
                        equal = true;
                        break;
                    }
                }
                if(!equal) {
                    return false;
                }
            }
            return true;
        }
    };
    struct window {
        QHash<QString, node> func;
        window() {
            func = QHash<QString, node>();
        }

        void insert(QString name, std::function<void(QObject*, bool)> fun, QList<int> keys) {
            if(func.find(name) == func.end()) {
                for(auto iter=func.begin(); iter!=func.end(); iter++) {
                    if(iter->isKeyEqual(keys)) {
                        qWarning() << "按键重复\n位置：key_manager::window::insert " + iter.key() + " " + name;
                        return;
                    }
                }
                func[name] = node(fun, keys);
            } else {
                qWarning("按键命名重复\n位置Key_manager::add_event");
            }
        }

        void findAndRun(bool is_enter, int key) {
            for(auto iter=func.begin(); iter!=func.end(); iter++) { //无论有键按下还是松开都会让原来的键失效
                if(iter->isKeyEqual(availiableKey) && iter->func != NULL) {
                    iter->func(iter->receiver, false);
                    break;
                }
            }
            if(is_enter) {
                availiableKey.append(key);
            } else {
                availiableKey.removeOne(key);
            }
            QList<int>  availiable = availiableKey;
            for(auto iter=func.begin(); iter!=func.end(); iter++) {
                if(iter->isKeyEqual(availiableKey) && iter->func != NULL) {
                    iter->func(iter->receiver, true);
                    break;
                }
            }
        }
    };
    struct listener_data {
        bool is_begin;
        IKeyListener* listener;
    };

    static QList<int> availiableKey;
    static QHash<QString, window> allKey;
    static QList<QString> keySettings;
    static QSet<QString> usingWindowSet;
    static QList<listener_data> listeners;
    static bool isWindowChange;
    static QList<GlobalKeyItem> globalKeys;
public:
    static QHash<int, QString> keyType;
};

#endif // KEY_MANAGER_H
