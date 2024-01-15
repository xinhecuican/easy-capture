#ifndef ARROWLAYER_H
#define ARROWLAYER_H
#include "layerbase.h"
#include "../Widgets/expandbutton.h"
#include <QtMath>

class ArrowLayer : public LayerBase
{
public:
    ArrowLayer(const QPointF& beginPt,
               const QPointF& endPt,
               const QString& name,
               ILayerControl* manager,
               QGraphicsItem* parent=nullptr);
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = nullptr) override;
    QRectF boundingRect()const override;
    QPainterPath shape()const override;
    void setParameter(const QString &name, const QVariant &var) override;
    void reset() override;
    void hoverEnterEvent(QGraphicsSceneHoverEvent *event) override;
    void hoverLeaveEvent(QGraphicsSceneHoverEvent *event) override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
    void setLine(const QPointF& beginPt, const QPointF& endPt);
    void getFocus() override;
    void loseFocus() override;
    void setEndPoint(const QPointF& point);
    int getZValue() const override;
    int type() const override;
    void prepareSave() override;
    bool contains(const QPointF& point)const override;
    void onDelete(const QPointF &point) override;
    void setStyle(const PaintData& data);

private slots:
    void posChangeFunc(ExpandButton::ButtonDirection dir, qreal x, qreal y);
private:
    void createArrow();
    void showButtons();
    void hideButtons();

private:
    QPointF arrow_points[3];
    QPointF beginPt;
    QPointF endPt;
    QPointF point;
    ExpandButton* beginButton;
    ExpandButton* endButton;
    QCursor outCursor;
    PaintData data;
    int index;
    bool press;
    const float ExtRefArrowLenght = 10;//箭头末端大小的长度，
    const float ExtRefArrowDegrees = M_PI / 6;//箭头末端顶角的一半
};

#endif // ARROWLAYER_H
