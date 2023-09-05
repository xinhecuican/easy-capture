#ifndef PICTURE_LAYER_H
#define PICTURE_LAYER_H
#include<QWidget>
#include<QPixmap>
#include<QPolygon>
#include<QPainter>
#include "Helper/plist.h"
#include "Paint/Data/Ilayer.h"
#include "paintlayer.h"
#include "Base/RecordElement.h"
#include "Paint/Data/stretch_button.h"
#include "Helper/plist.h"
#include <QGraphicsObject>
#include "rect_layer.h"
#include <QGraphicsPixmapItem>
#include "LayerItems/picitem.h"

class Picture_layer : public QGraphicsObject
{
    Q_OBJECT
public:
    enum DragPos{top_left, top_right, down_left, down_right};
    Picture_layer(QGraphicsItem* parent=nullptr);
    void setPixmap(const QPixmap& pix);
    QRectF boundingRect() const override;
    void reset();
    void setEnableMove(bool enable);
    void setDisableColor(int index, QColor color);
    void setSaveDisableColor(int index, QColor color);
    bool containsPicture();
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
               QWidget *widget = nullptr) override;
    void prepareSave();
    void endSave();
    int type() const override;
    Q_INVOKABLE void undoRedoSizeFunc(bool is_undo, QRectF before_rect, QRectF after_rect);
private:

    RectLayer* rect_layer;
    PicItem* picItem;
    QRectF bound;
    QList<QColor> disable_color;
    QList<QColor> save_disable_color;
    QRectF mask;
};

#endif // PICTURE_LAYER_H
