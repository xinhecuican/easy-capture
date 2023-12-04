#include "style_manager.h"
#include "../../Manager/uimanager.h"

Style_manager* Style_manager::_instance = NULL;

Style_manager::Style_manager() {
    data = PList<PaintData*>();
    now_data = UIManager::instance()->getPencilData();
    PaintData* temp_data = new PaintData();
    now_data.copy_to(temp_data);
    data.append(temp_data);
    is_change = false;
}

Style_manager::~Style_manager() {
    data.clear_all();
    _instance = NULL;
}

Style_manager* Style_manager::instance() {
    if(_instance == NULL) {
        _instance = new Style_manager();
    }
    return _instance;
}

void Style_manager::reset() {
    data.clear_all();
    now_data = UIManager::instance()->getPencilData();
    PaintData* temp_data = new PaintData();
    now_data.copy_to(temp_data);
    data.append(temp_data);
    is_change = false;
}

void Style_manager::change_color(QColor color) {
    now_data.color = color;
    is_change = true;
}

void Style_manager::change_back_color(QColor color) {
    now_data.backColor = color;
    is_change = true;
}

void Style_manager::change_width(double width) {
    now_data.width = width;
    is_change = true;
}

void Style_manager::change_cap_style(Qt::PenCapStyle style) {
    now_data.capStyle = style;
    is_change = true;
}

void Style_manager::change_join_style(Qt::PenJoinStyle style) {
    now_data.joinStyle = style;
    is_change = true;
}

void Style_manager::change_pen(PaintData data) {
    if(now_data.color != data.color)
        change_color(data.color);
    if(now_data.width != data.width)
        change_width(data.width);
    if(now_data.capStyle != data.capStyle)
        change_cap_style(data.capStyle);
    if(now_data.joinStyle != data.joinStyle)
        change_join_style(data.joinStyle);
}

PaintData* Style_manager::get() {
    if(!is_change) {
        return data.last();
    } else {
        PaintData* temp_data = new PaintData();
        now_data.copy_to(temp_data);
        data.push_back(temp_data);
        is_change = false;
        return temp_data;
    }
}

PaintData Style_manager::get_now() {
    if(is_change) {
        PaintData* temp_data = new PaintData();
        now_data.copy_to(temp_data);
        data.push_back(temp_data);
        is_change = false;
    }
    return now_data;
}
