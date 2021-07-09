#ifndef HISTORY_DATA_H
#define HISTORY_DATA_H
#include<QDateTime>
class History_data
{
public:
    enum save_type{Editable//可编辑
             ,Persist};//不可编辑
    qint64 time;
    QString file_name;
    save_type type;

};

#endif // HISTORY_DATA_H
