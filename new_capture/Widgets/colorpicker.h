#ifndef COLORPICKER_H
#define COLORPICKER_H
#include <QPixmap>
#include <QGraphicsObject>
#include "Paint/Widgets/Layers/baselayer.h"
#include "Helper/common.h"
#include "Helper/imagehelper.h"

class ColorPicker : public QGraphicsObject, public BaseLayer
{
public:
    ColorPicker(QGraphicsItem* parent=nullptr);
    void setPic(const QPixmap& pix);
    void setPosition(QPointF point);
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = nullptr) override;
    QPainterPath shape()const override;
    QRectF boundingRect() const override;
private:
    QImage image;
    QPointF point;
    QRect bound;
    QRect screenGeometry;
    int pixelWidthPX;
    int pixelHeightPX;
    int shadowWidth = 10;
    const int offsetX = 30;
    const int offsetY = 20;
    const int pixelWidthPT = 15;
    const int pixelHeightPT = 10;
};

#endif // COLORPICKER_H
