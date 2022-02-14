#ifndef PAINTITEM_H
#define PAINTITEM_H

#include<QHash>
#include<QPainterPath>
#include "Base/Record_element.h"
#include "Paint/Data/paint_data.h"
#include "Paint/Data/Ilayer.h"
#include<QWidget>
#include "Paint/Widgets/Recorder_element/paint_record.h"
#include "Paint/Data/Common.h"
#include<QGraphicsItem>

class PaintItem : public QGraphicsItem
{
public:
    PaintItem(paint_info info, QGraphicsItem* parent=nullptr);
    QRectF boundingRect() const override;
    void setPath(QPainterPath path);
    QPainterPath shape() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget=nullptr)override;
private:
    paint_info info;
};

#endif // PAINTITEM_H
