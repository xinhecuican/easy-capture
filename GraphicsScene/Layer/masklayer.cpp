#include "masklayer.h"
#include "../../Manager/config.h"
#include "../../Helper/math.h"
#include "../../Manager/uimanager.h"
#include "../../Helper/plist.h"

MaskLayer::MaskLayer(const QString& name,
                     ILayerControl* manager,
                     QGraphicsItem* parent)
    : ClipLayerBase(name, manager, parent)
{
    borderData = UIManager::instance()->getBorderData();
    backgroundColor = UIManager::instance()->getCaptureBackground();
    backgroundColor = QColor(0, 0, 0, 100);
    borderData.width = 2;
    colorPicker = new ColorPicker(manager, this);
    colorPicker->hide();
}

QRectF MaskLayer::boundingRect() const {
    return bound;
}

void MaskLayer::onImageSet() {
    bound = manager->getImage().rect();
}


void MaskLayer::mousePressEvent(QGraphicsSceneMouseEvent *event) {
    button = event->button();
    if(button != Qt::LeftButton || !enable)
        return;
    if(Config::getConfig<int>(Config::capture_mode) == Config::FREE_CAPTURE) {
        freeCapturePath.clear();
        freeCapturePath.moveTo(mapFromScene(event->scenePos()));
    }
    beginPoint = mapFromScene(event->scenePos());
    endPoint = beginPoint;
    isDrag = false;
    beginClip = true;
    for(int i=0; i<regions.size(); i++) {
        if(regions[i]->contain(regions[i]->mapFromParent(beginPoint))) {
            isDrag = true;
            break;
        }
    }
    for(int i=0; i<free_regions.size(); i++) {
        if(free_regions[i]->contain(regions[i]->mapFromParent(beginPoint))) {
            isDrag = true;
            break;
        }
    }
    if(!isDrag){
        regionChangeBegin(beginPoint);
    }
    emit boundChangeBegin();
}

void MaskLayer::mouseMoveEvent(QGraphicsSceneMouseEvent *event) {
    if(!isDrag || !enable) {
        update();
        regionMove(mapFromScene(event->scenePos()));
    }
    if(Config::getConfig<int>(Config::capture_mode) == Config::FREE_CAPTURE)
        freeCapturePath.lineTo(mapFromScene(event->scenePos()));
    else
        endPoint = mapFromScene(event->scenePos());

}

void MaskLayer::mouseReleaseEvent(QGraphicsSceneMouseEvent *event) {
    beginClip = false;
    QRectF new_rect;
    if(Config::getConfig<int>(Config::capture_mode) == Config::FREE_CAPTURE)
        new_rect = freeCapturePath.boundingRect();
    else
        new_rect = Math::buildRect(beginPoint, mapFromScene(event->scenePos()));
    if(new_rect.width() < 10 || new_rect.height() < 10 || isDrag || button != Qt::LeftButton || !enable) {
        regionChanged();
        return;
    }
    if(Config::getConfig<int>(Config::capture_mode) == Config::FREE_CAPTURE) {
        freeCapturePath.lineTo(mapFromScene(event->scenePos()));
        freeCapturePath.closeSubpath();
        freeCapturePath = freeCapturePath.simplified();
        QList<QPolygonF> polygons = freeCapturePath.toSubpathPolygons();

        for(QPolygonF polygon : polygons) {
            ClipRegion* new_region = new ClipRegion(polygon, true, this);
            free_regions.append(new_region);
        }
    } else {
        addRegion(new_rect);
    }
    update();
    regionChanged();
}

void MaskLayer::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) {
    if(!isSaving) {
        painter->fillRect(bound, QColor(0, 0, 0, 1));
        QPen pen;
        pen.setColor(borderData.color);
        pen.setWidth(borderData.width);
        painter->setPen(pen);

        QPainterPath origin;
        origin.addRect(bound);
        QPainterPath path;
        path.setFillRule(Qt::WindingFill);
        if(!isDrag && beginClip && button == Qt::LeftButton) {
            if(Config::getConfig<int>(Config::capture_mode) == Config::FREE_CAPTURE) {
                QPen tmp_pen = pen;
                pen.setColor(QColor(41, 141, 255));
                pen.setWidth(3);
                painter->setPen(pen);
                painter->drawPath(freeCapturePath);
                pen = tmp_pen;
                painter->setPen(pen);
            } else
                path.addRect(Math::buildRect(beginPoint, endPoint));
        }
        QPolygonF polygon;
        for(ClipRegion* region : regions) {
            polygon = polygon.united(region->getPolygon());
        }
        for(ClipRegion* region : free_regions) {
            polygon = polygon.united(region->getPolygon());
        }
        path.addPolygon(polygon);
        path = path.simplified();
        painter->drawPath(path);
        origin = origin.subtracted(path);
        painter->fillPath(origin, backgroundColor);
    }
}

void MaskLayer::addRegion(const QPolygonF& polygon) {
    PList<ClipRegion*> intersect_path;
    auto iter = regions.begin();
    int i=0;
    while(iter != regions.end()) {
        if(iter.i->t()->intersect(polygon)) {
            intersect_path.append(regions.at(i));
            iter = regions.erase(iter);
        } else {
            i++;
            iter++;
        }
    }
    ClipRegion* newRegion = new ClipRegion(polygon, this);
    for(ClipRegion* region : intersect_path) {
        newRegion->united(region);
    }
    intersect_path.clear_all();
    regions.append(newRegion);
    connect(newRegion, &ClipRegion::regionChangeBegin, this, [=](QPointF point){
        regionChangeBegin(point);
    });
    connect(newRegion, &ClipRegion::regionChange, this, [=]() {
        regionChanged();
    });
    connect(newRegion, &ClipRegion::regionMove, this, [=](QPointF point){
        regionMove(point);
    });
    connect(newRegion, &ClipRegion::regionPress, this, [=](){
        emit boundChangeBegin();
    });
}

void MaskLayer::prepareSave() {
    for(ClipRegion* region : regions) {
        region->hideNormal();
    }
    for(ClipRegion* region: free_regions) {
        region->hideNormal();
    }
    ClipLayerBase::prepareSave();
}

void MaskLayer::endSave() {
    clearRegion();
    ClipLayerBase::endSave();
}

QRectF MaskLayer::getClipRect() {
    QPainterPath path = getClipPath();
    return path.boundingRect();
}

void MaskLayer::clearRegion() {
    for(ClipRegion* region : regions) {
        delete region;
    }
    for(ClipRegion* region : free_regions) {
        delete region;
    }
    regions.clear();
    free_regions.clear();
}

QPainterPath MaskLayer::getClipPath() {
    QPainterPath path;
    for(int i=0; i<regions.size(); i++) {
        QPolygonF temp_polygon = regions[i]->getPolygon();
        path.addPolygon(temp_polygon);
    }
    for(int i=0; i<free_regions.size(); i++) {
        QPolygonF temp_polygon = free_regions[i]->getPolygon();
        path.addPolygon(temp_polygon);
    }
    return path;
}

int MaskLayer::getRegionCount() {
    return regions.size() + free_regions.size();
}

void MaskLayer::regionChangeBegin(const QPointF &point) {
    colorPicker->show();
    colorPicker->setPosition(point);
    emit boundChangeBegin();
}

void MaskLayer::regionMove(const QPointF &point) {
    colorPicker->setPosition(point);
}

void MaskLayer::regionChanged() {
    colorPicker->hide();
    emit boundChange(getClipRect());
}

void MaskLayer::reset() {
    isDrag = false;
    beginClip = false;
    clearRegion();
    freeCapturePath.clear();
}

int MaskLayer::getZValue() const {
    return 1;
}

void MaskLayer::setEnable(bool enable, int index){
    if(index == type() && enable){
        this->enable = true;
        for(auto& region : regions){
            region->setEnable(true);
        }
    }
    else{
        this->enable = false;
        for(auto& region : regions){
            region->setEnable(false);
        }
    }
}

int MaskLayer::type() const{
    return 102404;
}
