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
#include <QPainterPath>

class PaintItem : public QGraphicsObject {
public:
    PaintItem() {}
    PaintItem(PaintInfo info, QGraphicsItem* parent=nullptr);
    QRectF boundingRect() const override;
    void addPoint(QPointF point, bool updateGlobal=false);
    QPainterPath shape() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget=nullptr)override;
    void paintLine(QPainter* painter);
    void setEnd();
private:
    PaintInfo info;
    QVector<QPointF> points;
    QPen pen;
    QRectF boundRect;
    QPainterPath path;
    bool _isEnd;
};

#endif // PAINTITEM_H
