#ifndef PICLAYER_H
#define PICLAYER_H
#include "cliplayerbase.h"
#include "rectlayer.h"

class PicLayer : public ClipLayerBase
{
public:
    PicLayer(const QString& name, ILayerControl* manager, bool enableBound=true, QGraphicsItem* parent=nullptr);
    void setParameter(const QString &name, const QVariant &var) override;
    void prepareSave() override;
    void endSave() override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget=nullptr)override;
    QRectF boundingRect() const override;
    QPainterPath shape() const override;
    void onImageSet() override;
    void reset() override;
    void setEnable(bool enable, int index=0) override;
    int type() const override;
    void setStyle(const PaintData& data);
    QRectF getSaveRect() override;
    QRectF getClipRect()override;
    QPainterPath getClipPath()override;
private:
    bool enableBound;
    RectLayer* rectLayer;
    QRectF mask;
};

#endif // PICLAYER_H
