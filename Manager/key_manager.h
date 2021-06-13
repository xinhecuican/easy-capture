#ifndef KEY_MANAGER_H
#define KEY_MANAGER_H
#include <QString>
#include<functional>
#include<QList>
#include<cstdarg>

class Key_manager
{
public:
    Key_manager();
    static void add_event(QString obj_name, std::function<void()> const &f, int key_count, ...);
    static void on_key_count_change();
    static void key_enter(Qt::Key key);
    static void key_release(Qt::Key key);
private:
    struct node
    {
        std::function<void()> func;
        QList<Qt::Key> keys;
        node(std::function<void()> const &f, QList<Qt::Key> &key)
        {
            func = f;
            keys = key;
        }
    };
    struct window
    {
        QString name;
        QList<node> func;
    };
    static QList<window> all_key;
};

#endif // KEY_MANAGER_H
