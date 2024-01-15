#ifndef HISTORY_H
#define HISTORY_H
#include<QColor>
#include<QDomDocument>
#include "../GraphicsScene/ilayercontrol.h"

struct HistoryData{
    ILayerControl::SaveType type;
    qint64 time;
    QString file_name;
};

class History
{
public:
    History();
    ~History();
    static History* instance();
    void log(ILayerControl::SaveType type, QString file_name);
    void log_color(QColor color);
    void remove_color(QColor color);
    void load(QString file_name);
    QString get_last_directory();
    int get_history_num();
    HistoryData get(int i);
    bool is_change;
    void update();
    QList<QColor> get_color();
    void setVideoSavePath(QString path);
    QString getVideoSavePath();
private:
    static History* _instance;
    QList<HistoryData> data;
    void load_info();
    void init();
    int history_num;
    int color_num;
    QList<QColor> colors;
    QDomDocument doc;
    QString last_directory;
    QString videoSavePath;
};

#endif // HISTORY_H
