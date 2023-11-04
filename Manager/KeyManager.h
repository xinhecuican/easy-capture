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
#include "Helper/Template.h"
#include <QKeyEvent>

class KeyManager {
    DECLARE_INSTANCE(KeyManager);
    KeyManager();
    void addFunc(QObject* receiver, QString window_name, QString obj_name, std::function<void(bool)> const &f);
    void addKey(QString window_name, QString obj_name, QList<int> keys);
    void keyEnter(QString name, QKeyEvent* event);
    void keyRelease(QString name, QKeyEvent* event);
    void addKeyListener(IKeyListener* listener);
    void removeKeyListener(IKeyListener* listener);
    void save();
    void load();
    void reset();
    void updateAll();
    void updateGlobalKey();
    void onWindowChangeBegin(QString oldWindow);
    void onWindowChangeEnd(QString newWindow);
    void onWindowClose(QString windowName);
    QList<int> getKeys(QString window_name, QString key_name);
    void setKeys(QString window_name, QString key_name, QList<int> keys);
    QList<QString> getWindowNames();
    QList<QString> getKeyNames(QString window_name);
    QList<QString> detectKeyConflict(QString window_name, QString key_name, QList<int> keys);
    void clearKeys(QString name);

    quint32 nativeKeycode(Qt::Key keycode);
    quint32 nativeModKeyCode(Qt::Key keycode);
    int qtKeyCode(quint32 keycode);
    QList<ATOM> getGlobalKeyId();
    QList<QString> getGlobalKeyName();
    QString getGlobalKeyName(int index);
    void addGlobalKey(QString name, int modKey, int key, bool init);
    void registerGlobalKey(QString name);
    void unRegisterGlobalKey(QString name);
    int getGlobalModKey(int index);
    int getGlobalKey(int index);
    bool isGloablKeyRegistered(int index);
    void unRegisterAll();
    void registerAll();
    void updateKey(QString windowName, QString keyName, QList<int> keys);
    void updateGlobalKey(QString name, int key, int modKey);
    bool testGlobalKey(int index);
    bool testGlobalKey(int key, int modKey);
private:
    void loadDefault();
    void onKeyCountChange(QString name, bool is_enter, int key);//按键按下或松开时的处理函数
private:
    struct GlobalKeyItem {
        bool registered;
        bool init;
        ATOM keyId;
        int modKey;
        int key;
        QString name;
        GlobalKeyItem():
            registered(false),
            init(false),
            modKey(0),
            key(0)
        {

        }
    };
    struct LocalKeyItem {
        std::function<void(bool)> func;
        QList<int> keys;
        LocalKeyItem() {
            func = NULL;
            keys = QList<int>();
        }
        LocalKeyItem(std::function<void(bool)> const &f, QList<int> &key) {
            func = f;
            keys = key;
        }

        bool isKeyEqual(const QList<int> &key) {
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
    struct KeyWindow {
        QHash<QString, LocalKeyItem> func;
        QList<int> currentKeys;
        KeyWindow() {
            func = QHash<QString, LocalKeyItem>();
        }

        void insert(QString name, std::function<void(bool)> fun, QList<int> keys) {
            if(func.find(name) == func.end()) {
                for(auto iter=func.begin(); iter!=func.end(); iter++) {
                    if(iter->isKeyEqual(keys)) {
                        qWarning() << "按键重复\n位置：key_manager::window::insert " + iter.key() + " " + name;
                        return;
                    }
                }
                func[name] = LocalKeyItem(fun, keys);
            } else {
                qWarning("按键命名重复\n位置Key_manager::add_event");
            }
        }

        void run(int key, bool isEnter){
            if(!isEnter){
                for(auto iter=func.begin(); iter!=func.end(); iter++){
                    if(iter.value().isKeyEqual(currentKeys)){
                        iter.value().func(false);
                    }
                }
                currentKeys.removeOne(key);
            }
            else{
                currentKeys.append(key);
                for(auto iter=func.begin(); iter!=func.end(); iter++){
                    if(iter.value().isKeyEqual(currentKeys)){
                        iter.value().func(true);
                    }
                }
            }
        }
    };
    struct listener_data {
        bool is_begin;
        IKeyListener* listener;
    };
    QHash<QString, KeyWindow> allKey;
    QList<listener_data> listeners;
    QSet<quint32> listenerKeySet;
    QList<GlobalKeyItem> globalKeys;
    QHash<QString, KeyWindow> defaultKeys;
    QList<GlobalKeyItem> defaultGlobalKeys;
    bool isLoadDefault=false;
public:
    static QHash<int, QString> keyType;
};

#endif // KEY_MANAGER_H
