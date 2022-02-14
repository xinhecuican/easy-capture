#ifndef PICTURE_LAYER_H
#define PICTURE_LAYER_H
#include<QWidget>
#include<QPixmap>
#include<QPolygon>
#include<QPainter>
#include "new_capture/Widgets/Ipoint_position_change.h"
#include "new_capture/Widgets/capture_region.h"
#include "Helper/plist.h"
#include "Paint/Data/Ilayer.h"
#include "paint_layer.h"
#include "Base/Record_element.h"
#include "Paint/Data/stretch_button.h"
#include "Helper/plist.h"
#include "Paint/Data/button_group.h"
#include <QGraphicsItem>
#include "rect_layer.h"

class Picture_layer : public QGraphicsItem
{
public:
    enum DragPos{top_left, top_right, down_left, down_right};
    Picture_layer(QGraphicsItem* parent=nullptr);
    void setPixmap(const QPixmap& pix);
    QRectF boundingRect() const override;
    void reset();
    void setEnableMove(bool enable);
    void setDisableColor(int index, QColor color);
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = nullptr) override;
private:
    Rect_layer* rect_layer;
    QPixmap pix;
    QRectF bound;
    QHash<int, QColor> disable_color;
};

#endif // PICTURE_LAYER_H
