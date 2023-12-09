#ifndef BLURLAYER_H
#define BLURLAYER_H
#include "layerbase.h"
#include <QBitArray>

class BlurLayer : public LayerBase
{
public:
    BlurLayer(const QString& name,
              ILayerControl* manager,
              QGraphicsItem* parent=nullptr);
    void reset() override;
    void setParameter(const QString &name, const QVariant &var) override;
    QRectF boundingRect()const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = nullptr) override;
    void onImageSet() override;
    void addPoint(const QPoint& point);
    void setEnable(bool enable, int index=0) override;
    int type() const override;
private:
    QBitArray use;
    QRectF bound;
    int range;
    int windowSize;
};

#endif // BLURLAYER_H
