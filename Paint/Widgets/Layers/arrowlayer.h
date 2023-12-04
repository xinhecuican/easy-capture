#ifndef ARROWLAYER_H
#define ARROWLAYER_H
#include<QGraphicsObject>
#include "baselayer.h"
#include "LayerItems/expandbutton.h"
#include<QtMath>

class ArrowLayer : public QGraphicsObject, public BaseLayer {
    Q_OBJECT
public:
    ArrowLayer(QGraphicsItem* parent, QPointF begin_point, QPointF end_point);
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = nullptr) override;
    QRectF boundingRect()const override;
    QPainterPath shape()const override;
    void hoverEnterEvent(QGraphicsSceneHoverEvent *event) override;
    void hoverLeaveEvent(QGraphicsSceneHoverEvent *event) override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
    void setLine(QPointF begin_point, QPointF end_point);
    void getFocusFunc() override;
    void loseFocusFunc() override;
    int type() const override;
    Q_INVOKABLE void hideNormal() override;
    Q_INVOKABLE void showNormal() override;
    Q_INVOKABLE bool acceptFocus() override;
private slots:
    void posChangeFunc(direction dir, qreal x, qreal y);
private:
    void createArrow();
    void showButtons();
    void hideButtons();
    QPointF arrow_points[3];
    QPointF begin_point;
    QPointF end_point;
    QPointF point;
    ExpandButton* begin_button;
    ExpandButton* end_button;
    QCursor out_cursor;
    bool is_focus;
    bool enable_move;
    PaintData data;
    const float ExtRefArrowLenght = 10;//箭头末端大小的长度，
    const float ExtRefArrowDegrees = M_PI / 6;//箭头末端顶角的一半
};

#endif // ARROWLAYER_H
