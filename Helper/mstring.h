#ifndef MSTRING_H
#define MSTRING_H
#include<QString>
#include<QHash>

class MString
{
public:
    MString();
    static void load_from_file(const QString& path);
    static QString search(const QString& id);
private:
    static QHash<QString, QString> strings;
    static QList<QString> loading_list;
};

#endif // MSTRING_H
