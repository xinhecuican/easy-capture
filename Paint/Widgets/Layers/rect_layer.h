#ifndef RECT_LAYER_H
#define RECT_LAYER_H
#include"Paint/Data/Ilayer.h"
#include "Paint/Data/button_group.h"

class Rect_layer : public Ilayer
{
public:
    Rect_layer(QWidget* parent, QRect rect);
    void paint(QPainter *painter, QList<QColor> disable_color, bool is_save) override;
    void set_name(QString name) override;
    QPolygon bounded_rect() override;
    QString get_name() override;
    bool focuseable() override;
    // TODO: 完成get_focus lose_focus
private:
    QString name;
    QRect bound;
    QWidget* parent;
    Button_group* group;
};

#endif // RECT_LAYER_H
