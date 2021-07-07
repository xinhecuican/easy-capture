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
    void reset();
    void change_color(QColor color);
    void change_width(double width);
    Paint_data* get();
    Paint_data get_now();
private:
    static Style_manager* _instance;
    PList<Paint_data*> data;
    Paint_data now_data;
    bool is_change;//设置改变了就需要加入data备份中
};

#endif // STYLE_MANAGER_H
