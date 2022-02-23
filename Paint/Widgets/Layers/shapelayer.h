#ifndef SHAPELAYER_H
#define SHAPELAYER_H
#include<QGraphicsItem>
#include<QGraphicsObject>
#include "Paint/Data/Common.h"
#include "baselayer.h"

class ShapeLayer : public QGraphicsObject
{
    Q_OBJECT
public:
    ShapeLayer(QGraphicsItem* parent=nullptr);
    void reset();
    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = nullptr) override;
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;
    void setEnable(bool enable);
    void setShape(SHAPE_TYPE shape);
    void setGrabFocus(bool enable);
    void deleteShape();
    void prepareSave();
    void endSave();
public slots:
    void onRequestFocus(BaseLayer* object, QGraphicsObject* object2);

private:
    void setLayersState();
    bool childContains(QPointF point);
    void setFocus(QGraphicsObject* object, BaseLayer* func);
    bool is_enable;
    bool is_press;
    bool is_focus;
    SHAPE_TYPE shape;
    QPointF begin_point;
    QGraphicsObject* focus_item;
    BaseLayer* focus_item_func;
};

#endif // SHAPELAYER_H
