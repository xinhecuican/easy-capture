#ifndef FONTDATA_H
#define FONTDATA_H
#include <QFont>
#include <QColor>

class FontData {
public:
    FontData();
    FontData(QFont font, QColor color);
    QFont font;
    QColor color;
};

#endif // FONTDATA_H
