#ifndef ROOTLAYER_H
#define ROOTLAYER_H
#include <QGraphicsItem>

class PaintArea;

class RootLayer : public QGraphicsItem
{
public:
    RootLayer(PaintArea* area, QGraphicsItem* parent=nullptr);
    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = nullptr) override;
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;
    void setErase(bool enable);
private:
    void deleteChild(const QPointF& point);
    PaintArea* area;
    bool erase;
    bool press;
};

#endif // ROOTLAYER_H
