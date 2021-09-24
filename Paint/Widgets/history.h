#ifndef HISTORY_H
#define HISTORY_H
#include "Paint/Data/History_data.h"
#include<QColor>
#include<QDomDocument>

class History
{
public:
    History();
    ~History();
    static History* instance();
    void log(History_data::save_type type, QString file_name);
    void log_color(QColor color);
    void remove_color(QColor color);
    void load(QString file_name);
    QString get_last_directory();
    int get_history_num();
    History_data get(int i);
    bool is_change;
    void update();
    QList<QColor> get_color();
private:
    static History* _instance;
    QList<History_data> data;
    void load_info();
    void init();
    int history_num;
    int color_num;
    QList<QColor> colors;
    QDomDocument doc;
    QString last_directory;
};

#endif // HISTORY_H
