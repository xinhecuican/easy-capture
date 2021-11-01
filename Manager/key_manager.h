#ifndef KEY_MANAGER_H
#define KEY_MANAGER_H
#include <QString>
#include<functional>
#include<QList>
#include<cstdarg>
#include "Helper/debug.h"
#include "Helper/EnumReflect.h"
#include "IKey_listener.h"

class Key_manager
{
public:

    Key_manager();
    static void add_func(QString window_name, QString obj_name, std::function<void(bool)> const &f);
    static void add_key(QString window_name, QString obj_name, QList<int> keys);
    static void on_key_count_change(bool is_enter, int key);//按键按下或松开时的处理函数
    static void key_enter(int key);
    static void key_release(int key);
    static bool is_contains_window(QString window_name);
    static void add_key_listener(IKey_listener* listener);
    static void remove_key_listener(IKey_listener* listener);
    static void save();
    static void load();
    static void update_all();
    static void on_window_change(QString old_window, QString new_window);
    static QList<int> get_keys(QString window_name, QString key_name);
    static void set_keys(QString window_name, QString key_name, QList<int> keys);
    static QList<QString> get_window_names();
    static QList<QString> get_key_names(QString window_name);
    static QList<QString> detect_key_conflict(QString window_name, QString key_name, QList<int> keys);
private:
    struct node
    {
        std::function<void(bool)> func;
        QList<int> keys;
        node()
        {
            func = NULL;
            keys = QList<int>();
        }
        node(std::function<void(bool)> const &f, QList<int> &key)
        {
            func = f;
            keys = key;
        }

        bool is_key_equal(QList<int> &key)
        {
            for(int i=0; i<keys.size(); i++)
            {
                bool equal = false;
                for(int k=0; k<key.size(); k++)
                {
                    if(keys[i] == key[k])
                    {
                        equal = true;
                        break;
                    }
                }
                if(!equal)
                {
                    return false;
                }
            }
            return true;
        }
    };
    struct window
    {
        QHash<QString, node> func;
        window()
        {
            func = QHash<QString, node>();
        }

        void insert(QString name, std::function<void(bool)> fun, QList<int> keys)
        {
            if(func.find(name) == func.end())
            {
                for(auto iter=func.begin(); iter!=func.end(); iter++)
                {
                    if(iter->is_key_equal(keys))
                    {
                        Debug::debug_print_warning("按键重复\n位置：key_manager::window::insert");
                        return;
                    }
                }
                func[name] = node(fun, keys);
            }
            else
            {
                Debug::debug_print_warning("按键命名重复\n位置Key_manager::add_event");
            }
        }

        void find_and_run(bool is_enter, int key)
        {
            for(auto iter=func.begin(); iter!=func.end(); iter++)//无论有键按下还是松开都会让原来的键失效
            {
                if(iter->is_key_equal(availiable_key) && iter->func != NULL)
                {
                    iter->func(false);
                    break;
                }
            }
            if(is_enter)
            {
                availiable_key.append(key);
            }
            else
            {
                availiable_key.removeOne(key);
            }
            QList<int>  availiable = availiable_key;
            for(auto iter=func.begin(); iter!=func.end(); iter++)
            {
                if(iter->is_key_equal(availiable_key) && iter->func != NULL)
                {
                    iter->func(true);
                    break;
                }
            }
        }
    };
    struct listener_data
    {
        bool is_begin;
        IKey_listener* listener;
    };

    static QList<int> availiable_key;
    static QHash<QString, window> all_key;
    static QList<QString> key_settings;
    static QSet<QString> using_window_set;
    static QList<listener_data> listeners;

public:
    static QHash<int, QString> key_type;
};

#endif // KEY_MANAGER_H
