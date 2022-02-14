#include "picture_layer.h"
#include "Paint/Widgets/recorder.h"
#include<QBitmap>
#include<QGraphicsSceneHoverEvent>


Picture_layer::Picture_layer(QGraphicsItem* parent): QGraphicsItem(parent)
{
    rect_layer = new Rect_layer(this, boundingRect());
//    rect_layer->setAcceptHoverEvents(false);
    setAcceptHoverEvents(true);
    rect_layer->show();
    setFlag(QGraphicsItem::ItemIsSelectable);
}

void Picture_layer::setPixmap(const QPixmap &pix)
{
    this->pix = pix;
    this->bound.setHeight(pix.height());
    this->bound.setWidth(pix.width());
    rect_layer->setBounding(this->boundingRect());
}

QRectF Picture_layer::boundingRect() const
{
    return this->bound;
}

void Picture_layer::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    QRectF src(0, 0, bound.width(), bound.height());
    painter->drawPixmap(QPointF(0, 0), pix, src);
}

void  Picture_layer::reset()
{
    pix = QPixmap();
}

void Picture_layer::setEnableMove(bool enable)
{
    rect_layer->setEnableSizeChange(enable);
}

void Picture_layer::setDisableColor(int index, QColor color)
{
    if(disable_color.contains(index))
    {
        disable_color.remove(index);
    }
    else
    {
        disable_color[index] = color;
    }
}
