#ifndef BUTTONBOXLAYER_H
#define BUTTONBOXLAYER_H
#include <QGraphicsObject>
#include "Paint/Widgets/Layers/LayerItems/expandbutton.h"
#include "Paint/Widgets/Layers/baselayer.h"

class ButtonBoxLayer : public QGraphicsObject, public BaseLayer
{
public:
    ButtonBoxLayer(QGraphicsItem* parent=nullptr);
    QPainterPath shape()const override;
    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = nullptr) override;
    Q_INVOKABLE void hideNormal() override;
    Q_INVOKABLE void showNormal() override;
private:
    QRectF bound;
};

#endif // BUTTONBOXLAYER_H
