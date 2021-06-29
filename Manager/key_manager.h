#ifndef KEY_MANAGER_H
#define KEY_MANAGER_H
#include <QString>
#include<functional>
#include<QList>
#include<cstdarg>
#include "Helper/debug.h"

class Key_manager
{
public:
    Key_manager();
    static void add_func(QString window_name, QString obj_name, std::function<void(bool)> const &f);
    static void add_key(QString window_name, QString obj_name, QList<int> keys);
    static void on_key_count_change(bool is_enter, int key);
    static void key_enter(int key);
    static void key_release(int key);
    static bool is_contains_window(QString window_name);
    static void save();
    static void load();
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
            for(int i=0; i<key.size(); i++)
            {
                bool equal = false;
                for(int k=0; k<keys.size(); k++)
                {
                    if(keys[k] == key[i])
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
            for(auto iter=func.begin(); iter!=func.end(); iter++)
            {
                if(iter->is_key_equal(availiable_key) && iter->func != NULL)
                {
                    iter->func(false);
                    break;
                }
            }
            QList<int> temp_list = QList<int>(availiable_key);
            if(is_enter)
            {
                temp_list.append(key);
            }
            else
            {
                temp_list.removeOne(key);
            }
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
    static QList<int> availiable_key;
    static QHash<QString, window> all_key;
    static QList<QString> key_settings;
    static QSet<QString> using_window_set;
};

#endif // KEY_MANAGER_H
