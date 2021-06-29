#include "key_manager.h"
#include<QDebug>
#include "window_manager.h"
#include "Helper/debug.h"
#include<QDomDocument>

Key_manager::Key_manager()
{

}

QHash<QString, Key_manager::window> Key_manager::all_key = QHash<QString, window>();
QList<int> Key_manager::availiable_key = QList<int>();
QSet<QString> Key_manager::using_window_set = QSet<QString>();
QList<QString> Key_manager::key_settings = {
    "Capture_window:leave;16777216",//Escape,可以通过Qt::Key_Escape查看键值
    "Capture_window:one_window;49",
    "Capture_window:multi_window_separate;50",
    "Capture_window:multi_window_combine;51",
    "Capture_window:move_all;16777249"//ctrl
};

void Key_manager::add_key(QString window_name, QString obj_name, QList<int> keys)
{
    if(all_key.find(window_name) == all_key.end())
    {
        window temp = window();
        temp.insert(obj_name, NULL, keys);
        all_key[window_name] = temp;
    }
    else
    {
        all_key[window_name].insert(obj_name, NULL, keys);
    }
}

void Key_manager::add_func(QString window_name, QString obj_name, std::function<void(bool)> const &f)
{
    if(!using_window_set.contains(window_name))
    {
        using_window_set.insert(window_name);
    }
    if(all_key.contains(window_name) && all_key[window_name].func.contains(obj_name))
    {
        all_key[window_name].func[obj_name].func = f;
    }
    else
    {
        Debug::show_error_message("未找到对应按键\n位置：Key_manager::add_func");
    }
}

void Key_manager::on_key_count_change(bool is_enter, int key)
{
    QString now_window = Window_manager::get_now_window();
    window window_key = all_key[now_window];
    window_key.find_and_run(is_enter, key);
}

void Key_manager::key_enter(int key)
{
    on_key_count_change(true, key);
    availiable_key.append(key);
}

void Key_manager::key_release(int key)
{
    on_key_count_change(false, key);
    availiable_key.removeOne(key);

}

bool Key_manager::is_contains_window(QString window_name)
{
    return using_window_set.contains(window_name);
}

void Key_manager::save()
{
    QFile file("Data/keys.xml");
    if(!file.open(QIODevice::ReadWrite | QIODevice::Truncate))
    {
        Debug::show_error_message("按键配置文件无法打开\n位置：Key_manager::save");
        return;
    }
    QDomDocument doc;
    QDomProcessingInstruction instruction; //添加处理命令
    instruction=doc.createProcessingInstruction("xml","version=\"1.0\" encoding=\"UTF-8\"");
    doc.appendChild(instruction);
    //添加根节点
    QDomElement root=doc.createElement("key_setting");
    doc.appendChild(root);

    QHashIterator<QString, window> iter(all_key);
    while(iter.hasNext())
    {
        iter.next();
        QDomElement element = doc.createElement("window");
        element.setAttribute("name", iter.key());
        QHashIterator<QString, node> iter2(iter.value().func);
        while(iter2.hasNext())
        {
            iter2.next();
            QDomElement element2 = doc.createElement("bind_key");
            element2.setAttribute("name", iter2.key());
            for(int i=0; i<iter2.value().keys.size(); i++)
            {
                QDomElement element3 = doc.createElement("key");
                element3.setAttribute("id", iter2.value().keys[i]);
                element2.appendChild(element3);
            }
            element.appendChild(element2);
        }
        root.appendChild(element);
    }

    QTextStream out_stream(&file);
    doc.save(out_stream,4); //缩进4格
    file.close();
}

void Key_manager::load()
{
    QFile file("Data/keys.xml");
    if(!file.open(QIODevice::ReadOnly))
    {
        for(int i=0; i<key_settings.size(); i++)
        {
            int first_index = key_settings[i].indexOf(':');
            int second_index = key_settings[i].indexOf(';');
            QList<int> using_key = QList<int>();
            QString temp;
            for(int k=second_index+1; k<key_settings[i].size(); k++)
            {
                if(key_settings[i][k] == ',')
                {
                    using_key.append(temp.toInt());
                    temp.clear();
                }
                else
                {
                    temp.append(key_settings[i][k]);
                }
            }
            using_key.append(temp.toInt());
            add_key(key_settings[i].mid(0, first_index),
                    key_settings[i].mid(first_index+1, second_index-first_index-1), using_key);
        }
    }
    else
    {
        QDomDocument doc;
        if(!doc.setContent(&file))
        {
            file.close();
            return;
        }
        file.close();

        QDomElement root = doc.documentElement();
        if(root.tagName().compare("key_setting") == 0)
        {
            QDomNodeList childs = root.childNodes();
            for(int i=0; i<childs.size(); i++)
            {
                QDomElement element = childs.at(i).toElement();
                QString window_name = element.attribute("name");
                QDomNodeList list = element.childNodes();
                for(int k=0; k<list.size(); k++)
                {
                    QDomElement element2 = list.at(k).toElement();
                    QString obj_name = element2.attribute("name");
                    QDomNodeList key_list = element2.childNodes();
                    QList<int> keys = QList<int>();
                    for(int j=0; j<key_list.size(); j++)
                    {
                        keys.append(key_list.at(j).toElement().attribute("id").toInt());
                    }
                    add_key(window_name, obj_name, keys);
                }
            }
        }
    }
}
