#ifndef RECT_LAYER_H
#define RECT_LAYER_H
#include"Paint/Data/Ilayer.h"

class Rect_layer : public Ilayer
{
public:
    Rect_layer(QWidget* parent, QRect rect);
    void paint(QPainter *painter, QList<QColor> disable_color, bool is_save) override;
    void set_name(QString name) override;
    QRect bounded_rect() override;
    QString get_name() override;
private:
    QString name;
    QRect bound;
    QWidget* parent;
};

#endif // RECT_LAYER_H
