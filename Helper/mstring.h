#ifndef MSTRING_H
#define MSTRING_H
#include<QString>
#include<QHash>

class MString
{
public:
    MString();
    static void load_from_file(QString path);
    static QString search(QString id);
private:
    static QHash<QString, QString> strings;
    static QList<QString> loading_list;
};

#endif // MSTRING_H
