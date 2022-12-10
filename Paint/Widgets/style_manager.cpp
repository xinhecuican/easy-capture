#include "style_manager.h"

Style_manager::Style_manager()
{
    data = PList<Paint_data*>();
    now_data = Style_manager::default_pencil;
    Paint_data* temp_data = new Paint_data();
    now_data.copy_to(temp_data);
    data.append(temp_data);
    is_change = false;
}

Style_manager::~Style_manager()
{
    data.clear_all();
    _instance = NULL;
}

Style_manager* Style_manager::_instance = NULL;

Paint_data Style_manager::default_pencil = Paint_data(QColor(150, 50, 50), 3);
Paint_data Style_manager::default_highlighter = Paint_data(
            QColor(255, 255, 0, 100), 12, QColor(255, 255, 255), Qt::RoundCap, Qt::RoundJoin);


Style_manager* Style_manager::instance()
{
    if(_instance == NULL)
    {
        _instance = new Style_manager();
    }
    return _instance;
}

void Style_manager::reset()
{
    data.clear_all();
    now_data = default_pencil;
    Paint_data* temp_data = new Paint_data();
    now_data.copy_to(temp_data);
    data.append(temp_data);
    is_change = false;
}

void Style_manager::change_color(QColor color)
{
    now_data.color = color;
    is_change = true;
}

void Style_manager::change_back_color(QColor color)
{
    now_data.back_color = color;
    is_change = true;
}

void Style_manager::change_width(double width)
{
    now_data.width = width;
    is_change = true;
}

void Style_manager::change_cap_style(Qt::PenCapStyle style)
{
    now_data.cap_style = style;
    is_change = true;
}

void Style_manager::change_join_style(Qt::PenJoinStyle style)
{
    now_data.join_style = style;
    is_change = true;
}

void Style_manager::change_pen(Paint_data data)
{
    if(now_data.color != data.color)
        change_color(data.color);
    if(now_data.width != data.width)
        change_width(data.width);
    if(now_data.cap_style != data.cap_style)
        change_cap_style(data.cap_style);
    if(now_data.join_style != data.join_style)
        change_join_style(data.join_style);
}

Paint_data* Style_manager::get()
{
    if(!is_change)
    {
        return data.last();
    }
    else
    {
        Paint_data* temp_data = new Paint_data();
        now_data.copy_to(temp_data);
        data.push_back(temp_data);
        is_change = false;
        return temp_data;
    }
}

Paint_data Style_manager::get_now()
{
    if(is_change)
    {
        Paint_data* temp_data = new Paint_data();
        now_data.copy_to(temp_data);
        data.push_back(temp_data);
        is_change = false;
    }
    return now_data;
}
