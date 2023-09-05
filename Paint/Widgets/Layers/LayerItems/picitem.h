#ifndef PICITEM_H
#define PICITEM_H
#include <QGraphicsObject>

class PicItem : public QGraphicsItem
{
public:
    PicItem(QGraphicsItem* parent=nullptr);
    void reset();
    void setEnableMove(bool enable);
    void setDisableColor(int index, QColor color);
    void setSaveDisableColor(int index, QColor color);
    bool containsPicture();
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
               QWidget *widget = nullptr) override;
    QRectF boundingRect() const override;
    void setPix(const QPixmap& pix);
    void setMask(QRectF mask);
private:
    QPixmap pix;
    QRectF bound;
    QRectF mask;
    QList<QColor> disable_color;
    QList<QColor> save_disable_color;
    QPixmap mask_pix;
};

#endif // PICITEM_H
