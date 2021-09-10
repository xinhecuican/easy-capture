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
    virtual void erase_and_paint(QPoint point, QPainter* painter, QList<QColor> disable_color)override;
    virtual void paint(QPainter* painter, QList<QColor> disable_color, bool is_save)override;
    virtual int add_data(Paint_data* style, QPainterPath path)override;
    virtual void set_name(QString name)override;
    virtual QRect bounded_rect()override;
    virtual bool redo(int index = 0) override;
    virtual bool undo(int index = 0) override;
    virtual QString get_name()override;
private:
    QString name;
    QTextEdit* edit;
    QRect bound_rect;
};

#endif // TEXT_LAYER_H
