#include "grabcutlayer.h"
#include <opencv2/opencv.hpp>
#include "../../Helper/imagehelper.h"
#include "../../Helper/debug.h"
#include <QTextStream>

GrabcutLayer::GrabcutLayer(const QString& name,
                           ILayerControl* manager,
                           QGraphicsItem* parent)
    : LayerBase(name, manager, parent),
      background(true),
      enableShow(false)
{
    handler = new GrabcutHandler(this);
    connect(handler, &GrabcutHandler::finish, this, [=](QImage mask){
        manager->maskImage(mask);
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
        pen.setColor(QColor(255, 0, 0, 128));
        pen.setWidth(5);
        painter->setPen(pen);
        for(int i=0; i<foregroundPaths.size(); i++){
            painter->drawPath(foregroundPaths[i]);
        }
        pen.setColor(QColor(0, 0, 255, 128));
        painter->setPen(pen);
        for(int i=0; i<backgroundPaths.size(); i++){
            painter->drawPath(backgroundPaths[i]);
        }
        if(background) pen.setColor(QColor(0, 0, 255, 128));
        else pen.setColor(QColor(255, 0, 0, 128));
        painter->setPen(pen);
        painter->drawPath(currentPath);
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

}

int GrabcutLayer::getZValue() const{
    return 2;
}

void GrabcutLayer::setEnableShow(){
    enableShow = true;
}
