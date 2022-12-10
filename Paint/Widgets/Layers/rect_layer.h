#ifndef RECT_LAYER_H
#define RECT_LAYER_H
#include <QGraphicsItem>
#include "LayerItems/expandbutton.h"
#include "LayerItems/scrollitem.h"
#include<QGraphicsObject>
#include "baselayer.h"

class Rect_layer : public QGraphicsObject, public BaseLayer
{
    Q_OBJECT
public:
    enum RECT_STYLE{NORMAL, RED, CUSTOM};
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
    void setLimit(QRectF limit);
    void setEnableMove(bool enable);
    void setEnableSizeChange(bool enable);
    void setEnableScroll(bool enable);
    void getFocusFunc() override;
    void loseFocusFunc() override;
    void setStyle(RECT_STYLE style, Paint_data data=Paint_data(QColor(161, 47, 47), 3));
    bool contains(const QPointF& point)const override;
    int type() const override;
    Q_INVOKABLE void hideNormal() override;
    Q_INVOKABLE void showNormal() override;
    Q_INVOKABLE bool acceptFocus() override;
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
    ScrollItem* scroll_item;
    QPointF begin_point;
    RECT_STYLE style;
    Paint_data data;
    bool enable_move;
    bool enable_size_change;
    bool enable_scroll;
    bool force_show;
};

#endif // RECT_LAYER_H
