#include "fontdata.h"

FontData::FontData() {

}

FontData::FontData(QFont font, QColor color) {
    this->font = font;
    this->color = color;
}
