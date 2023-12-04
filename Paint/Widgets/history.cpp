#include "history.h"
#include<QFile>
#include<QDomDocument>
#include<QTextStream>
#include "../../Helper/debug.h"
#include<QDebug>
#include "../../Manager/config.h"
#include<QDir>

History::History()
{
    data = QList<History_data>();
    history_num = 0;
    last_directory = "C:/";
    is_change = true;
    color_num = 0;
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
    doc = QDomDocument();
    if(!file.exists("Data/history.xml"))
    {
        if(!file.open(QIODevice::ReadWrite))
        {
            qWarning("历史文件打开失败");
            return;
        }
        qint64 time = QDateTime::currentDateTime().currentSecsSinceEpoch();
        History_data temp_data;
        temp_data.type = type;
        temp_data.time = time;
        temp_data.file_name = file_name;
        data.append(temp_data);
        init();
        if(type == History_data::Persist)
        {
//            int index = file_name.lastIndexOf('/');
//            last_directory = file_name.mid(0, index+1);
            last_directory = file_name;
            QDomNodeList list = doc.elementsByTagName("history");
            list.at(0).toElement().setAttribute("directory", last_directory);
        }
        QTextStream out_stream(&file);
        doc.save(out_stream,4); //缩进4格
        file.close();
    }
    else
    {
        if(!file.open(QIODevice::ReadWrite))
        {
            qWarning("历史文件打开失败");
            return;
        }
        if(!doc.setContent(&file))
        {
            file.close();
            return;
        }
        file.close();
        qint64 time = QDateTime::currentDateTime().currentSecsSinceEpoch();
        int max_sum = Config::getConfig<int>(Config::history_num);
        QDomElement root = doc.documentElement();
        QDomNodeList list = doc.elementsByTagName("history");
        QDomElement history = list.at(0).toElement();
        QDomElement element = doc.createElement("history_node");
        element.setAttribute("name", file_name);
        QDomElement element2 = doc.createElement("type");
        element2.setAttribute("sum", QString::number(type));
        QDomElement element3 = doc.createElement("time");
        element3.setAttribute("sum", QString::number(time));
        element.appendChild(element2);
        element.appendChild(element3);
        history.appendChild(element);
        History_data temp_data;
        temp_data.type = type;
        temp_data.time = time;
        temp_data.file_name = file_name;
        data.append(temp_data);
        if(type == History_data::Persist)
        {
//            int index = file_name.lastIndexOf('/');
//            last_directory = file_name.mid(0, index+1);
            last_directory = file_name;
            history.setAttribute("directory", last_directory);
        }

        if(history_num + 1 <= max_sum)
        {
            history.setAttribute("history_num", ++history_num);
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

            history.removeChild(history.firstChild());
            data.removeAt(0);
        }
        else
        {
            history.removeChild(history.firstChild());
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

void History::log_color(QColor color)
{
    is_change = true;
    QFile file("Data/history.xml");
    doc = QDomDocument();
    if(!file.exists("Data/history.xml"))
    {
        if(!file.open(QIODevice::ReadWrite))
        {
            qWarning("历史文件打开失败");
            return;
        }
        colors.append(color);
        color_num++;
        init();
        QTextStream out_stream(&file);
        doc.save(out_stream,4); //缩进4格
        file.close();
    }
    else
    {
        if(!file.open(QIODevice::ReadWrite))
        {
            qWarning("历史文件打开失败");
            return;
        }
        if(!doc.setContent(&file))
        {
            file.close();
            return;
        }
        file.close();

        colors.append(color);
        color_num++;
        QDomElement root = doc.documentElement();
        QDomElement default_colors = doc.elementsByTagName("default_colors").at(0).toElement();
        QDomElement child_element = doc.createElement("color");
        child_element.setAttribute("r", color.red());
        child_element.setAttribute("g", color.green());
        child_element.setAttribute("b", color.blue());
        default_colors.appendChild(child_element);
        default_colors.setAttribute("num", color_num);

        if(!file.open(QFile::WriteOnly|QFile::Truncate))
        {
            return;
        }
        QTextStream out_stream(&file);
        doc.save(out_stream,4); //缩进4格
        file.close();
    }
}

void History::remove_color(QColor color)
{
    is_change = true;
    QFile file("Data/history.xml");
    doc = QDomDocument();
    if(!file.open(QIODevice::ReadWrite))
    {
        qWarning("历史文件打开失败");
        return;
    }
    if(!doc.setContent(&file))
    {
        file.close();
        return;
    }
    file.close();
    for(int i=0; i<colors.size(); i++)
    {
        if(colors[i].red() == color.red() && colors[i].green() == color.green() && colors[i].blue() == color.blue())
        {
            colors.removeAt(i);
            color_num--;
            i--;
        }
    }
    QDomElement root = doc.documentElement();
    QDomElement default_colors = root.elementsByTagName("default_colors").at(0).toElement();
    root.removeChild(default_colors);
    default_colors = doc.createElement("default_colors");
    default_colors.setAttribute("num", color_num);
    for(int i=0; i<colors.size(); i++)
    {
        QDomElement element = doc.createElement("color");
        element.setAttribute("r", colors[i].red());
        element.setAttribute("g", colors[i].green());
        element.setAttribute("b", colors[i].blue());
        default_colors.appendChild(element);
    }
    root.appendChild(default_colors);
    if(!file.open(QFile::WriteOnly|QFile::Truncate))
    {
        return;
    }
    QTextStream out_stream(&file);
    doc.save(out_stream,4); //缩进4格
    file.close();
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
        doc = QDomDocument();
        if(!doc.setContent(&file))
        {
            file.close();
            return;
        }
        file.close();

        QDomElement root = doc.documentElement();
        if(root.tagName().compare("root") == 0)
        {
            QDomNodeList base_childs = root.childNodes();
            for(int i=0; i<base_childs.size(); i++)
            {
                QDomElement base_element = base_childs.at(i).toElement();
                if(base_element.tagName().compare("history") == 0)
                {
                    history_num = base_element.attribute("history_num").toInt();
                    last_directory = base_element.attribute("directory");
                    videoSavePath = base_element.attribute("video_save_path");
                    QDomNodeList childs = base_element.childNodes();
                    for(int i=0; i<childs.size(); i++)
                    {
                        QDomElement element = childs.at(i).toElement();
                        if(element.tagName().compare("history_node") == 0)
                        {
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
                else if(base_element.tagName().compare("default_colors") == 0)
                {
                    color_num = base_element.attribute("num").toInt();
                    QDomNodeList childs = base_element.childNodes();
                    for(int i=0; i<childs.size(); i++)
                    {
                        QDomElement element = childs.at(i).toElement();
                        if(element.tagName().compare("color") == 0)
                        {
                            int r = element.attribute("r").toInt();
                            int g = element.attribute("g").toInt();
                            int b = element.attribute("b").toInt();
                            QColor color = QColor(r, g, b);
                            colors.append(color);
                        }
                    }
                }
            }
        }
    }
}

QString History::get_last_directory()
{
    return last_directory;
}

QString History::getVideoSavePath(){
    return videoSavePath;
}

void History::setVideoSavePath(QString path){
    videoSavePath = path;

    is_change = true;
    QFile file("Data/history.xml");
    doc = QDomDocument();
    if(!file.exists("Data/history.xml"))
    {
        if(!file.open(QIODevice::ReadWrite))
        {
            qWarning("历史文件打开失败");
            return;
        }
        init();
        QDomNodeList list = doc.elementsByTagName("history");
        list.at(0).toElement().setAttribute("video_save_path", videoSavePath);
        QTextStream out_stream(&file);
        doc.save(out_stream,4); //缩进4格
        file.close();
    }
    else
    {
        if(!file.open(QIODevice::ReadWrite))
        {
            qWarning("历史文件打开失败");
            return;
        }
        if(!doc.setContent(&file))
        {
            file.close();
            return;
        }
        file.close();
        QDomElement root = doc.documentElement();
        QDomNodeList list = doc.elementsByTagName("history");
        QDomElement history = list.at(0).toElement();
        history.setAttribute("video_save_path", videoSavePath);
        if(!file.open(QFile::WriteOnly|QFile::Truncate))
        {
            return;
        }
        QTextStream out_stream(&file);
        doc.save(out_stream,4); //缩进4格
        file.close();
    }
}

int History::get_history_num()
{
    return history_num;
}

History_data History::get(int i)
{
    return data[i];
}

void History::update()
{
    QFile file("Data/history.xml");
    if(!file.open(QIODevice::ReadOnly))
    {
        return;
    }
    doc = QDomDocument();
    if(!doc.setContent(&file))
    {
        file.close();
        return;
    }
    file.close();
    QDomElement root = doc.documentElement();
    if(root.tagName().compare("root") != 0 && root.tagName().compare("history") == 0)
    {
        last_directory = root.attribute("directory");
        QDomNodeList childs = root.childNodes();
        for(int i=0; i<childs.size(); i++)
        {
            QDomElement element = childs.at(i).toElement();
            if(element.tagName().compare("history_node") == 0)
            {
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
        doc = QDomDocument();
        init();
        doc.elementsByTagName("history").at(0).toElement().setAttribute("directory", last_directory);
        if(!file.open(QFile::WriteOnly|QFile::Truncate))
        {
            return;
        }
        QTextStream out_stream(&file);
        doc.save(out_stream,4); //缩进4格
        file.close();
    }
}

void History::init()
{
    QDomProcessingInstruction instruction; //添加处理命令
    instruction=doc.createProcessingInstruction("xml","version=\"1.0\" encoding=\"UTF-8\"");
    doc.appendChild(instruction);
    QDomElement root = doc.createElement("root");
    QDomElement history_node = doc.createElement("history");

    QDomElement color_node = doc.createElement("default_colors");
    root.appendChild(history_node);
    root.appendChild(color_node);

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
        history_node.appendChild(element);
    }
    for(int i=0; i<colors.size(); i++)
    {
        color_num++;
        QDomElement element = doc.createElement("color");
        element.setAttribute("r", colors[i].red());
        element.setAttribute("g", colors[i].green());
        element.setAttribute("b", colors[i].blue());
        color_node.appendChild(element);
    }
    color_node.setAttribute("num", color_num);
    history_node.setAttribute("history_num", history_num);
    doc.appendChild(root);
}

QList<QColor> History::get_color()
{
    return colors;
}
