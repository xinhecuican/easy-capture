#include "history.h"
#include<QFile>
#include<QDomDocument>
#include<QTextStream>
#include "Helper/debug.h"
#include<QDebug>
#include "Manager/config.h"
#include<QDir>

History::History()
{
    data = QList<History_data>();
    history_num = 0;
    last_directory = "C:/";
    is_change = true;
    load_info();
}

History::~History()
{
    _instance = NULL;
}

History* History::_instance = NULL;

History* History::instance()
{
    if(_instance == NULL)
    {
        _instance = new History();
    }
    return _instance;
}

void History::log(History_data::save_type type, QString file_name)
{
    is_change = true;
    QFile file("Data/history.xml");
    QDomDocument doc;
    if(!file.exists("Data/history.xml"))
    {
        if(!file.open(QIODevice::ReadWrite))
        {
            Debug::debug_print_warning("历史文件打开失败");
            return;
        }
        QDomProcessingInstruction instruction; //添加处理命令
        instruction=doc.createProcessingInstruction("xml","version=\"1.0\" encoding=\"UTF-8\"");
        doc.appendChild(instruction);
        //添加根节点
        QDomElement root=doc.createElement("history");
        doc.appendChild(root);
        for(int i=0; i<data.size(); i++)
        {
            history_num++;
            QDomElement element = doc.createElement("history_node");
            element.setAttribute("name", data[i].file_name);
            QDomElement element2 = doc.createElement("type");
            element2.setAttribute("sum", QString::number(data[i].type));
            QDomElement element3 = doc.createElement("time");
            element3.setAttribute("sum", QString::number(data[i].time));
            element.appendChild(element2);
            element.appendChild(element3);
            root.appendChild(element);
        }
        QDomElement element = doc.createElement("history_node");
        element.setAttribute("name", file_name);
        QDomElement element2 = doc.createElement("type");
        element2.setAttribute("sum", QString::number(type));
        QDomElement element3 = doc.createElement("time");
        qint64 time = QDateTime::currentDateTime().currentSecsSinceEpoch();
        element3.setAttribute("sum", QString::number(time));
        element.appendChild(element2);
        element.appendChild(element3);
        History_data temp_data;
        temp_data.type = type;
        temp_data.time = time;
        temp_data.file_name = file_name;
        data.append(temp_data);
        root.appendChild(element);
        root.setAttribute("history_num", ++history_num);
        if(type == History_data::Persist)
        {
            int index = file_name.lastIndexOf('/');
            last_directory = file_name.mid(0, index+1);
            root.setAttribute("directory", last_directory);
        }
        QTextStream out_stream(&file);
        doc.save(out_stream,4); //缩进4格
        file.close();
    }
    else
    {
        if(!file.open(QIODevice::ReadWrite))
        {
            Debug::debug_print_warning("历史文件打开失败");
            return;
        }
        if(!doc.setContent(&file))
        {
            file.close();
            return;
        }
        file.close();
        qint64 time = QDateTime::currentDateTime().currentSecsSinceEpoch();
        int max_sum = Config::get_config(Config::history_num);
        QDomElement root = doc.documentElement();
        QDomElement element = doc.createElement("history_node");
        element.setAttribute("name", file_name);
        QDomElement element2 = doc.createElement("type");
        element2.setAttribute("sum", QString::number(type));
        QDomElement element3 = doc.createElement("time");
        element3.setAttribute("sum", QString::number(time));
        element.appendChild(element2);
        element.appendChild(element3);
        root.appendChild(element);
        History_data temp_data;
        temp_data.type = type;
        temp_data.time = time;
        temp_data.file_name = file_name;
        data.append(temp_data);
        if(type == History_data::Persist)
        {
            int index = file_name.lastIndexOf('/');
            last_directory = file_name.mid(0, index+1);
            root.setAttribute("directory", last_directory);
        }

        if(history_num + 1 <= max_sum)
        {
            root.setAttribute("history_num", ++history_num);
        }
        else if(History::instance()->data[0].type == History_data::Editable)
        {
            QString file_name = History::instance()->data[0].file_name;
            if(file_name.contains("Data/Temp"))
            {
                int index = file_name.lastIndexOf('/');
                file_name = file_name.mid(0, index+1);
                QDir dir(file_name);
                dir.removeRecursively();
            }

            root.removeChild(root.firstChild());
            data.removeAt(0);
        }
        else
        {
            root.removeChild(root.firstChild());
            data.removeAt(0);
        }
        if(!file.open(QFile::WriteOnly|QFile::Truncate))
        {
            return;
        }
        QTextStream out_stream(&file);
        doc.save(out_stream,4); //缩进4格
        file.close();
    }
}

void History::load_info()
{
    QFile file("Data/history.xml");
    if(!file.open(QIODevice::ReadOnly))
    {
        return;
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
        if(root.tagName().compare("history") == 0)
        {
            history_num = root.toElement().attribute("history_num").toInt();
            last_directory = root.toElement().attribute("history");
            QDomNodeList childs = root.childNodes();
            for(int i=0; i<childs.size(); i++)
            {
                QDomElement element = childs.at(i).toElement();
                QString file_name = element.attribute("name");
                QDomNodeList list = element.childNodes();
                qint64 time;
                int type;
                for(int k=0; k<list.size(); k++)
                {
                    QDomElement element2 = list.at(k).toElement();
                    QString tag_name = element2.tagName();
                    if(tag_name.compare("type") == 0)
                    {
                        type = element2.attribute("sum").toInt();
                    }
                    else if(tag_name.compare("time") == 0)
                    {
                        time = element2.attribute("sum").toLongLong();
                    }
                }
                History_data history_data;
                history_data.time = time;
                history_data.type = (History_data::save_type)type;
                history_data.file_name = file_name;
                data.append(history_data);
            }
        }
    }
}

QString History::get_last_directory()
{
    return last_directory;
}

int History::get_history_num()
{
    return history_num;
}

History_data History::get(int i)
{
    return data[i];
}
