#include "grabcutlayer.h"
#include <opencv2/opencv.hpp>
#include "../../Helper/imagehelper.h"
#include "../../Helper/debug.h"
#include <QTextStream>
#include <QDateTime>
#include <QKeyEvent>

GrabcutLayer::GrabcutLayer(const QString& name,
                           ILayerControl* manager,
                           QGraphicsItem* parent)
    : LayerBase(name, manager, parent),
      background(true),
      enableShow(false)
{
    setFlags(QGraphicsItem::ItemIsFocusable);
    handler = new GrabcutHandler(this);
    connect(handler, &GrabcutHandler::finish, this, [=](QImage mask){
        manager->remaskImage(mask);
        update();
    });
}

void GrabcutLayer::addPoint(const QPointF &point, bool background, bool isEnd){
    this->background = background;
    if(currentPath.elementCount() == 0){
        currentPath.moveTo(point);
    }
    else{
        currentPath.lineTo(point);
    }
    bound = bound.united(currentPath.boundingRect());
    if(isEnd){
        if(background) backgroundPaths.append(currentPath);
        else foregroundPaths.append(currentPath);
        currentPath = QPainterPath();
        if(foregroundPaths.size() != 0 && backgroundPaths.size() != 0){
            handler->start(manager->getOriginImage(), foregroundPaths, backgroundPaths);
        }
    }
    update();
}

QRectF GrabcutLayer::boundingRect() const{
    return bound;
}

void GrabcutLayer::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget){
    if(enableShow){
        QPen pen;
        pen.setWidth(5);
        if(background) pen.setColor(QColor(0, 0, 255, 128));
        else pen.setColor(QColor(255, 0, 0, 128));
        painter->setPen(pen);
        painter->drawPath(currentPath);

        pen.setColor(QColor(255, 0, 0, 128));
        painter->setPen(pen);
        for(int i=0; i<foregroundPaths.size(); i++){
            painter->drawPath(foregroundPaths[i]);
        }
        pen.setColor(QColor(0, 0, 255, 128));
        painter->setPen(pen);
        for(int i=0; i<backgroundPaths.size(); i++){
            painter->drawPath(backgroundPaths[i]);
        }
    }
}

void GrabcutLayer::setEnable(bool enable, int index){
    enableShow = false;
    this->enable = false;
}

void GrabcutLayer::makeEnable(){
    enable = true;
}

int GrabcutLayer::type() const{
    return 102402;
}

void GrabcutLayer::reset(){
    enableShow = false;
    foregroundPaths.clear();
    backgroundPaths.clear();
    LayerBase::reset();
}

int GrabcutLayer::getZValue() const{
    return 2;
}

void GrabcutLayer::setEnableShow(){
    enableShow = true;
}

QRectF GrabcutLayer::getSaveRect(){
    return QRectF();
}

void GrabcutLayer::prepareSave(){
    LayerBase::prepareSave();
    enableShow = false;
}

void GrabcutLayer::deletePath(const QPointF &point){
    auto iter = foregroundPaths.begin();
    while(iter != foregroundPaths.end()){
        if(iter->contains(point)){
            iter = foregroundPaths.erase(iter);
        }
        else{
            iter++;
        }
    }

    auto iter2 = backgroundPaths.begin();
    while(iter2 != backgroundPaths.end()){
        if(iter2->contains(point)){
            iter2 = backgroundPaths.erase(iter2);
        }
        else{
            iter2++;
        }
    }
    if(foregroundPaths.size() != 0 && backgroundPaths.size() != 0){
        handler->start(manager->getOriginImage(), foregroundPaths, backgroundPaths);
    }
    update();
}

void GrabcutLayer::onDelete(const QPointF &point){
    deletePath(point);
}

void GrabcutLayer::startErase(){
    enableShow = true;
    update();
}
