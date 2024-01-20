#ifndef BACKGROUNDLAYER_H
#define BACKGROUNDLAYER_H
#include "layerbase.h"

class BackgroundLayer : public LayerBase
{
public:
    BackgroundLayer(const QString& name,
                    ILayerControl* manager,
                    QGraphicsItem* parent=nullptr);
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = nullptr) override;
    QRectF boundingRect()const override;
    void reset() override;
    void onImageSet() override;
    QRectF getSaveRect() override;
    int getZValue() const override;
private:
    QRectF bound;
};

#endif // BACKGROUNDLAYER_H
