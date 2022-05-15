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
    void setPix(const QPixmap& pix);
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = nullptr) override;
    void addPoint(QPoint point);
    void deletePoint(QPoint point);
    bool isSetPic();
    Q_INVOKABLE void hideNormal() override;
    Q_INVOKABLE void showNormal() override;
private:
    bool is_setpic;
    bool is_allocate;
    bool** mask_use;
    QPoint left_top;
    QImage pix;
    QImage mask;
    const int range=15;
};

#endif // BLUELAYER_H
