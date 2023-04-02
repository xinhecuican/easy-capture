#ifndef ILAYER_H
#define ILAYER_H
#include<QImage>
#include "paint_data.h"
#include "Base/RecordElement.h"
#include<QPixmap>
#include "Common.h"
#include<QDebug>
#include<QPainter>
#include<QWidget>

/**
 * @deprecated
 */
class Ilayer : public QWidget {
public:
    Ilayer(QWidget* parent=nullptr):QWidget(parent) {};
    virtual void paint(QPainter* painter, QList<QColor> disable_color, bool is_save)=0;
    virtual void setName(QString name)=0;
    virtual QPolygon bounded_rect()=0;
    virtual QString getName()=0;
    virtual ~Ilayer() {};
    virtual void getFocus() {};
    virtual void loseFocus() {};
    virtual bool focuseable() {
        return false;
    };

    virtual void mouseEnter(int key_code) {};
    virtual void mouseMove(int dx, int dy) {};
    virtual void mouseRelease() {};
    virtual void doubleClick() {};

    virtual void onSizeChange(int index, int dx, int dy) {};
    virtual void onPaintChange(int index, PaintInfo info) {};
signals:
    void update_rect(QRect rect);
};

#endif // ILAYER_H
