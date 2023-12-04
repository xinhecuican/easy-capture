#ifndef RECTLAYER_H
#define RECTLAYER_H
#include "layerbase.h"
#include "../Widgets/expandbutton.h"
#include "../Widgets/scrollitem.h"

class RectLayer : public LayerBase
{
    Q_OBJECT
public:
    RectLayer(const QRectF& rect, const QString& name, ILayerControl* manager, QGraphicsItem* parent=nullptr, bool enableResize=true);
    void setParameter(const QString &name, const QVariant &var) override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = nullptr) override;
    void prepareSave() override;
    void endSave() override;
    void reset() override;
    int getZValue() const override;
    QRectF boundingRect() const override;
    QPainterPath shape() const override;
    void getFocus() override;
    void loseFocus() override;
    void hoverEnterEvent(QGraphicsSceneHoverEvent *event) override;
    void hoverLeaveEvent(QGraphicsSceneHoverEvent *event) override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;

    void setEnableResize(bool enable);
    void setEnableScroll(bool enable);
    void setBounding(const QRectF& rect);
    void setLimit(const QRectF& limit);
    void setStyle(const PaintData& data);
    void showButtons();
    void hideButtons();
    bool contains(const QPointF& point)const override;
    void setBoundEffect();
    void setButtonFocus(bool enable);

signals:
    void sizeChange();
    void move(qreal dx, qreal dy);

private slots:
    void posChangeFunc(ExpandButton::ButtonDirection dir, qreal x, qreal y);
    void posToFunc(ExpandButton::ButtonDirection dir, qreal x, qreal y);

private:
    void paintStyle(QPainter* painter);
    void initButtons();
    QRectF rect;
    QMap<ExpandButton::ButtonDirection, ExpandButton*> buttons;
    ScrollItem* scrollItem;
    bool enableScroll;
    bool enableResize;
    bool buttonFocus;
    PaintData paintData;
    QPointF beginPoint;
    QCursor outCursor;
};

#endif // RECTLAYER_H
