#include "style_manager.h"

Style_manager::Style_manager()
{
    data = PList<Paint_data*>();
    now_data = Paint_data();
    now_data.width = 3;
    now_data.color = QColor(150, 50, 50);
    Paint_data* temp_data = new Paint_data();
    now_data.copy_to(temp_data);
    data.append(temp_data);
    is_change = false;
}

Style_manager* Style_manager::_instance = NULL;

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
}

void Style_manager::change_color(QColor color)
{
    now_data.color = color;
    is_change = true;
}

void Style_manager::change_width(double width)
{
    now_data.width = width;
    is_change = true;
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
    return now_data;
}
