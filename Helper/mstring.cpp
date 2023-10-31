#include "mstring.h"
#include<QFile>
#include "Manager/config.h"
#include<QDomDocument>
#include "debug.h"

MString::MString() {

}

QHash<QString, QString> MString::strings = QHash<QString, QString>();
QList<QString> MString::loading_list = QList<QString>();

/*
 * xml格式可以直接看:/Languages/Config/chinese.xml
 */
void MString::load_from_file(QString path) {
    strings.clear();
    QString language;
    int index = Config::getConfig<int>(Config::language);
    switch(index) {
    case Config::CHINESE:
        language = "chinese";
        break;
    case Config::ENGLISH:
        language = "english";
        break;
    default:
        language = "chinese";
        break;
    }
    QFile file(path + language + ".xml");
    if(!file.open(QIODevice::ReadOnly)) {
        return;
    }

    QDomDocument doc;
    if(!doc.setContent(&file)) {
        file.close();
        return;
    }
    file.close();

    QDomElement root = doc.documentElement();
    QDomNode node = root.firstChild();

    if(root.tagName() == "strings" && root.attribute("language").compare(language) == 0) {
        QDomNodeList list = root.childNodes();
        for(int i=0; i<list.size(); i++) {
            QDomNode n = list.at(i);
            if(n.isElement()) {
                QDomElement element = n.toElement();
                if(element.tagName() == "string") {
                    strings.insert(element.attribute("id"), element.attribute("text"));
                }
            }
        }
    }

    loading_list.append(path);
}

/*
 * string的id使用{}进行包围，方便翻译
 * 如果不想翻译直接大括号中没有内容即可
 * 可以不加括号，但是不能只写一半括号
 * string的id不能重复，建议通过https://www.random.org/strings/等随机字符串生成网站获得
 */
QString MString::search(QString id) {
    QString id_name = "";
    int end_index = 0;
    if(id[0] == "{") {
        for(int i=1; i<id.size(); i++) {
            if(id[i] == "}") {
                end_index = i;
                id_name = id.mid(1, i-1);
                break;
            }
        }
    } else {
        return id;
    }
    if(end_index == 0) {
        if(id[0] == "{")
            qWarning() << "字符串格式错误\n字符串为" + id;
        return id;
    } else {
        if(id_name != "" && strings.contains(id_name)) {
            return strings[id_name];
        } else {
            return id.mid(end_index + 1);
        }
    }
}
