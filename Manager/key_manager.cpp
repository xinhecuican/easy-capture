#include "key_manager.h"

Key_manager::Key_manager()
{

}

QList<Key_manager::window> Key_manager::all_key = QList<window>();

void Key_manager::add_event(QString obj_name, const std::function<void ()> &f, int key_count, ...)
{
    bool contains = false;
    for(int i=0; i<all_key.size(); i++)
    {
        if(all_key[i].name == obj_name)
        {
            contains = true;
            va_list args;
            va_start(args, key_count);
            QList<Qt::Key> keys;
            for(int i=0; i<key_count; ++i)
                keys.append(Qt::Key(va_arg(args, int)));
            va_end(args);
            all_key[i].func.append(node(f, keys));
        }
    }
}

void Key_manager::on_key_count_change()
{
    /**
      *TODO: 近期时间大概率是同一个窗口，因此可以从最近的窗口检索
      * 待完成
      */

}


