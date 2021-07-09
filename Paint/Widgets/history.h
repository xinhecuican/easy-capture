#ifndef HISTORY_H
#define HISTORY_H
#include "Paint/Data/History_data.h"

class History
{
public:
    History();
    ~History();
    static History* instance();
    void log(History_data::save_type type, QString file_name);
    void load(QString file_name);
    QString get_last_directory();
    int get_history_num();
    History_data get(int i);
    bool is_change;
private:
    static History* _instance;
    QList<History_data> data;
    void load_info();
    int history_num;
    QString last_directory;
};

#endif // HISTORY_H
