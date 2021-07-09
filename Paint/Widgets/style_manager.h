#ifndef STYLE_MANAGER_H
#define STYLE_MANAGER_H
#include "Helper/plist.h"
#include "Paint/Data/paint_data.h"
#include<QVector>

class Style_manager
{
public:
    Style_manager();
    static Style_manager* instance();
    ~Style_manager();
    void reset();
    void change_color(QColor color);
    void change_width(double width);
    void change_cap_style(Qt::PenCapStyle style);
    void change_join_style(Qt::PenJoinStyle style);
    void change_pen(Paint_data data);
    Paint_data* get();
    Paint_data get_now();
    static Paint_data default_pencil;
    static Paint_data default_highlighter;
private:
    static Style_manager* _instance;
    PList<Paint_data*> data;
    Paint_data now_data;
    bool is_change;//设置改变了就需要加入data备份中
};

#endif // STYLE_MANAGER_H
