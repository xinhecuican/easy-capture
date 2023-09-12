#ifndef BLURLAYER_H
#define BLURLAYER_H
#include <QGraphicsObject>
#include "baselayer.h"
#include <QImage>


class BlurLayer : public QGraphicsObject, public BaseLayer
{
    Q_OBJECT
public:
    BlurLayer(QGraphicsItem* parent);
    ~BlurLayer();
    QRectF boundingRect()const override;
    QPainterPath shape()const override;
    void setPix(const QPixmap& pix, QPoint pos);
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = nullptr) override;
    void addPoint(QPoint point);
    void deletePoint(QPoint point);
    bool isSetPic();
    void setRange(int range);
    void setUnitSize(int unit_size);
    void reset();
    int type() const override;
    Q_INVOKABLE void hideNormal() override;
    Q_INVOKABLE void showNormal() override;
    Q_INVOKABLE bool acceptFocus() override;
private:
    void clearMaskUse();
    struct BlurElement{
        QRect bound;
        QColor color;
    };
    void resetMaskSetting();
    bool is_setpic;
    bool is_allocate;
    bool** mask_use;
    QPoint left_top;
    QImage pix;
    QImage mask;
    int range=12;
    int unit_size=4;
    const int maskUnitSize=4;
    bool is_save;
    int maskUseWidth;
    int maskUseHeight;
    QRectF bound;
};

#endif // BLUELAYER_H
