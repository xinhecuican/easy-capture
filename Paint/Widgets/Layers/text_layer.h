#ifndef TEXT_LAYER_H
#define TEXT_LAYER_H
#include<QWidget>
#include<QLineEdit>
#include<QTextEdit>
#include "Paint/Data/Ilayer.h"

class Text_layer : QWidget, Ilayer
{
public:
    Text_layer(QRect bound_rect, QWidget* parent=nullptr);
    void mouseDoubleClickEvent(QMouseEvent *event) override;
    void erase_and_paint(QPoint point, QPainter* painter, QList<QColor> disable_color)override;
    void paint(QPainter* painter, QList<QColor> disable_color, bool is_save)override;
    int add_data(Paint_data* style, QPainterPath path)override;
    void set_name(QString name)override;
    QRect bounded_rect()override;
    QString get_name()override;
private:
    QString name;
    QTextEdit* edit;
    QRect bound_rect;
    QString str;
};

#endif // TEXT_LAYER_H
