#ifndef PAINTITEM_H
#define PAINTITEM_H

#include<QHash>
#include<QPainterPath>
#include "Base/RecordElement.h"
#include "Paint/Data/paint_data.h"
#include "Paint/Data/Ilayer.h"
#include<QWidget>
#include "Paint/Data/Common.h"
#include<QGraphicsItem>

class PaintItem : public QGraphicsObject {
public:
    PaintItem() {}
    PaintItem(PaintInfo info, QGraphicsItem* parent=nullptr);
    QRectF boundingRect() const override;
    void addPoint(QPointF point, bool needUpdate=false);
    QPainterPath shape() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget=nullptr)override;
private:
    PaintInfo info;
    QVector<QPointF> points;
    QPen pen;
    QRectF boundRect;
};

#endif // PAINTITEM_H
