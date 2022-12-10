#include "masklayer.h"
#include <QPainter>
#include<QGuiApplication>
#include <QScreen>
#include <QPainterPath>
#include <QGraphicsSceneMouseEvent>
#include "Manager/window_manager.h"
#include "Helper/math.h"
#include "Helper/plist.h"
#include "Manager/config.h"

MaskLayer::MaskLayer(QGraphicsItem* parent) : QGraphicsObject(parent)
{
    is_drag = false;
    is_save = false;
    begin_clip = false;
    is_enable = true;
    screen_rect = QGuiApplication::primaryScreen()->geometry();
}

void MaskLayer::reset()
{
    is_drag = false;
    begin_clip = false;
    is_save = false;
    clearRegion();
    free_capture_path.clear();
}

QPainterPath MaskLayer::shape() const
{
    QPainterPath path;
    path.addRect(screen_rect);
    return path;
}

QRectF MaskLayer::boundingRect() const
{
    return screen_rect;
}

void MaskLayer::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    button = event->button();
    if(button != Qt::LeftButton || !is_enable)
        return;
    if(Config::getConfig<bool>(Config::free_capture))
    {
        free_capture_path.clear();
        free_capture_path.moveTo(mapFromScene(event->scenePos()));
    }
    begin_point = mapFromScene(event->scenePos());
    is_drag = false;
    begin_clip = true;
    for(int i=0; i<regions.size(); i++)
    {
        if(regions[i]->contain(begin_point))
        {
            is_drag = true;
            break;
        }
    }
    for(int i=0; i<free_regions.size(); i++)
    {
        if(free_regions[i]->contain(begin_point))
        {
            is_drag = true;
            break;
        }
    }
}

void MaskLayer::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    if(!is_save)
    {
        QPen pen;
        pen.setColor(QColor(123, 123, 233));
        pen.setWidth(2);
        painter->setPen(pen);

        QPainterPath origin;
        origin.addRect(screen_rect);
        QPainterPath path;
        path.setFillRule(Qt::WindingFill);
        if(!is_drag && begin_clip && button == Qt::LeftButton)
        {
            if(Config::getConfig<bool>(Config::free_capture))
            {
                QPen tmp_pen = pen;
                pen.setColor(QColor(131, 175, 155));
                pen.setWidth(3);
                painter->setPen(pen);
                painter->drawPath(free_capture_path);
                pen = tmp_pen;
                painter->setPen(pen);
            }
            else
                path.addRect(Math::buildRect(begin_point, end_point));
        }
        QPolygonF polygon;
        for(ClipRegion* region : regions)
        {
            polygon = polygon.united(region->getPolygon());
        }
        for(ClipRegion* region : free_regions)
        {
            polygon = polygon.united(region->getPolygon());
        }
        path.addPolygon(polygon);
        path = path.simplified();
        painter->drawPath(path);
        origin = origin.subtracted(path);
        painter->fillPath(origin, QColor(0, 0, 0, 0x40));
    }
}

void MaskLayer::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    if(!is_drag || !is_enable)
    {
        update();
    }
    if(Config::getConfig<bool>(Config::free_capture))
        free_capture_path.lineTo(mapFromScene(event->scenePos()));
    else
        end_point = mapFromScene(event->scenePos());
}

void MaskLayer::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    begin_clip = false;
    QRectF new_rect;
    if(Config::getConfig<bool>(Config::free_capture))
        new_rect = free_capture_path.boundingRect();
    else
        new_rect = Math::buildRect(begin_point, mapFromScene(event->scenePos()));
    if(new_rect.width() < 10 || new_rect.height() < 10 || is_drag || button != Qt::LeftButton || !is_enable)
    {
        emit regionChanged();
        return;
    }
    if(Config::getConfig<bool>(Config::free_capture))
    {
        free_capture_path.lineTo(mapFromScene(event->scenePos()));
        free_capture_path.closeSubpath();
        free_capture_path = free_capture_path.simplified();
        QList<QPolygonF> polygons = free_capture_path.toSubpathPolygons();

        for(QPolygonF polygon : polygons)
        {
            ClipRegion* new_region = new ClipRegion(polygon, true, this);
            free_regions.append(new_region);
        }
    }
    else
    {
        addRegion(new_rect);
    }
    update();
    emit regionChanged();
}

void MaskLayer::addRegion(QPolygonF polygon)
{
    PList<ClipRegion*> intersect_path;
    auto iter = regions.begin();
    int i=0;
    while(iter != regions.end())
    {
        if(iter.i->t()->intersect(polygon))
        {
            intersect_path.append(regions.at(i));
            iter = regions.erase(iter);
        }
        else
        {
            i++;
            iter++;
        }
    }
    ClipRegion* new_region = new ClipRegion(polygon, this);
    for(ClipRegion* region : intersect_path)
    {
        new_region->united(region);
    }
    intersect_path.clear_all();
    regions.append(new_region);
    connect(new_region, &ClipRegion::regionChange, this, [=](){
        emit regionChanged();
    });
}

void MaskLayer::prepareSave()
{
    for(ClipRegion* region : regions)
    {
        region->hideNormal();
    }
    for(ClipRegion* region: free_regions)
    {
        region->hideNormal();
    }
    is_save = true;
}

void MaskLayer::endSave()
{
    is_save = false;
    clearRegion();
}

QRectF MaskLayer::getClipRect()
{
    QPainterPath path = getPath();
    return path.boundingRect();
}

void MaskLayer::clearRegion()
{
    for(ClipRegion* region : regions)
    {
        delete region;
    }
    for(ClipRegion* region : free_regions)
    {
        delete region;
    }
    regions.clear();
    free_regions.clear();
}

QPainterPath MaskLayer::getPath()
{
    QPainterPath path;
    for(int i=0; i<regions.size(); i++)
    {
        QPolygonF temp_polygon = regions[i]->getPolygon();
        path.addPolygon(temp_polygon);
    }
    for(int i=0; i<free_regions.size(); i++)
    {
        QPolygonF temp_polygon = free_regions[i]->getPolygon();
        path.addPolygon(temp_polygon);
    }
    return path;
}

int MaskLayer::getRegionCount()
{
    return regions.size() + free_regions.size();
}

void MaskLayer::setEnable(bool enable)
{
    is_enable = enable;
    for(ClipRegion* region : regions)
        region->setEnable(enable);
    for(ClipRegion* region : free_regions)
        region->setEnable(enable);
}
