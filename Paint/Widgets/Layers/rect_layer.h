#ifndef RECT_LAYER_H
#define RECT_LAYER_H
#include <QGraphicsItem>
#include "LayerItems/expandbutton.h"
#include<QGraphicsObject>
#include "baselayer.h"

class Rect_layer : public QGraphicsObject, BaseLayer
{
    Q_OBJECT
public:
    enum RECT_STYLE{NORMAL, RED};
    Rect_layer(QGraphicsItem* parent, QRectF rect);
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = nullptr) override;
    QPainterPath shape() const override;
    void hoverEnterEvent(QGraphicsSceneHoverEvent *event) override;
    void hoverLeaveEvent(QGraphicsSceneHoverEvent *event) override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
    QRectF boundingRect() const override;
    void showButtons();
    void hideButtons();
    void setBounding(QRectF rect);
    void setEnableMove(bool enable);
    void setEnableSizeChange(bool enable);
    void getFocusFunc() override;
    void loseFocusFunc() override;
    void setStyle(RECT_STYLE style);
    Q_INVOKABLE void hideNormal() override;
    Q_INVOKABLE void showNormal() override;
signals:
    void sizeChange();
    void move(qreal dx, qreal dy);
    void requestFocus(BaseLayer* object, QGraphicsObject* object2);

private slots:
    void posChangeFunc(direction dir, qreal x, qreal y);
    void posToFunc(direction dir, qreal x, qreal y);
private:
    void paintStyle(QPainter* painter);
    QCursor out_cursor;
    QRectF rect;
    QMap<direction, ExpandButton*> buttons;
    QPointF begin_point;
    RECT_STYLE style;
    bool enable_move;
    bool enable_size_change;
    bool force_show;
};

#endif // RECT_LAYER_H
