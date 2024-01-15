#ifndef MASKCOLORLAYER_H
#define MASKCOLORLAYER_H
#include "layerbase.h"

class MaskColorLayer : public LayerBase
{
public:
    MaskColorLayer(const QString& name,
                   ILayerControl* manager,
                   QGraphicsItem* parent=nullptr);
    void reset() override;
    void addMaskColor(const QColor& color);
    void removeMaskColor(int index);
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = nullptr) override;
    QRectF boundingRect()const override;
    int type() const override;
    void setEnable(bool enable, int index = 0) override;
    QList<QColor> getMaskColors();
    int getZValue() const override;
private:
    QList<QColor> colors;
};

#endif // MASKCOLORLAYER_H
