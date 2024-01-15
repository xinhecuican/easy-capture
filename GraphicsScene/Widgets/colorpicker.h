#ifndef COLORPICKER_H
#define COLORPICKER_H
#include <QPixmap>
#include <QGraphicsObject>
#include "../../Helper/common.h"
#include "../../Helper/imagehelper.h"
#include "../ilayercontrol.h"

class ColorPicker : public QGraphicsObject
{
public:
    ColorPicker(ILayerControl* manager, QGraphicsItem* parent=nullptr);
    void setPosition(QPointF point);
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = nullptr) override;
    QPainterPath shape()const override;
    QRectF boundingRect() const override;
private:
    QPointF point;
    QRect bound;
    QRect screenGeometry;
    ILayerControl* manager;
    int pixelWidthPX;
    int pixelHeightPX;
    int shadowWidth = 10;
    const int offsetX = 30;
    const int offsetY = 20;
    const int pixelWidthPT = 15;
    const int pixelHeightPT = 10;
};

#endif // COLORPICKER_H
