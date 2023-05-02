#ifndef FUNCDATA_H
#define FUNCDATA_H
#include <QString>
#include <functional>
struct FuncData{
    QString name;
    std::function<void ()> f;
};

#endif // FUNCDATA_H
