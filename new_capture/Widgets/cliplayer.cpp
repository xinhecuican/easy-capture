#include "cliplayer.h"
#include <QPainter>
#include <QGraphicsSceneMouseEvent>
#include "Helper/math.h"
#include "Helper/plist.h"
#include <QGuiApplication>
#include <QScreen>
#include "Manager/window_manager.h"
#include <QPixmap>
#include <QBitmap>

ClipLayer::ClipLayer(QGraphicsItem* parent) : QGraphicsObject(parent),
    is_drag(false),
    begin_clip(false),
    is_save(false)
{
    screen_rect = QGuiApplication::primaryScreen()->geometry();
    mask_layer = new MaskLayer(this);
}

QPainterPath ClipLayer::shape() const
{
    QPainterPath path;
    path.addRect(screen_rect);
    return path;
}

QRectF ClipLayer::boundingRect() const
{
    return screen_rect;
}

void ClipLayer::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    button = event->button();
    if(button == Qt::RightButton)
    {
        if(mask_layer->getRegionCount() == 0)
        {
            Window_manager::pop_window();
        }
        else
        {
            mask_layer->reset();
            update();
        }
    }
    else if(button == Qt::MidButton)
    {
        emit requestImage();
    }
}

void ClipLayer::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
}

void ClipLayer::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
}

void ClipLayer::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    if(!is_save)
    {
        painter->drawPixmap(QPointF(0, 0), pix);
    }
    else
    {
        painter->drawPixmap(QPointF(0, 0), savePix);
    }
}

void ClipLayer::prepareSave()
{
    mask_layer->prepareSave();
    is_save = true;
    QPainterPath path = mask_layer->getPath();
    QPixmap ans;
    QPixmap mask = QPixmap(pix.width(), pix.height());
    mask.fill(Qt::transparent);
    QPainter painter(&mask);
    painter.fillPath(path, QColor(1, 1, 1));
    savePix = pix;
    savePix.setMask(mask.createMaskFromColor(QColor(1, 1, 1), Qt::MaskOutColor));
}

void ClipLayer::endSave()
{
    is_save = false;
    mask_layer->endSave();
}

void ClipLayer::capture(QPixmap pix)
{
    QPainterPath path;
    QPolygonF polygon;
    QPixmap ans;
    QRect rect;
    for(int i=0; i<regions.size(); i++)
    {
        QPolygonF temp_polygon = regions[i]->getPolygon();
        path.addPolygon(temp_polygon);
        polygon = polygon.united(temp_polygon);
    }
    QPixmap mask = QPixmap(pix.width(), pix.height());
    mask.fill(Qt::transparent);
    QPainter painter(&mask);
    painter.fillPath(path, QColor(1, 1, 1));
    pix.setMask(mask.createMaskFromColor(QColor(1, 1, 1), Qt::MaskOutColor));
    rect = path.boundingRect().toRect();
    ans = pix.copy(rect);
    rect.moveTo(0, 0);
    Window_manager::change_window("Paint_window");
    Window_manager::get_window("Paint_window")->set_pic(ans, rect);
}

void ClipLayer::setPic(QPixmap pix)
{
    this->pix = pix;
}

QRectF ClipLayer::getClipRect()
{
    return mask_layer->getClipRect();
}

void ClipLayer::reset()
{
    is_save = false;
    mask_layer->reset();
}

void ClipLayer::sendRequestImage()
{
    emit requestImage();
}
