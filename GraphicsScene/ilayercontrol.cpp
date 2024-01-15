#include "ilayercontrol.h"
#include <QPainter>
#include <QDebug>
#include <QDateTime>

ILayerControl::ILayerControl()
{
    modified = false;
    imageValid = false;
}

QImage ILayerControl::getOriginImage() const{
    return image;
}

QImage ILayerControl::getImage() const{
    if(modified){
        return modifiedImage;
    }
    return image;
}

void ILayerControl::modifyImage(const QPoint &pos, const QColor& color){
    if(!modified){
        modifiedImage = image.copy();
    }
    modified = true;
    QRgb* line = (QRgb*)modifiedImage.scanLine(pos.y());
    line[pos.x()] = color.rgb();
//    modifiedImage.setPixel(pos, color.rgba());
}

bool ILayerControl::isImageValid(){
    return imageValid;
}

void ILayerControl::maskImage(const QImage &mask){
    if(!modified){
        modifiedImage = image.copy();
    }
    modified = true;
    for(int i=0; i<modifiedImage.height(); i++){
        QRgb* line = (QRgb*)modifiedImage.scanLine(i);
        const uchar* maskLine = mask.constScanLine(i);
        for(int k=0; k<modifiedImage.width(); k++){
            QRgb rgb = line[k];
            if(qAlpha(rgb) != 0){
                line[k] = qRgba(qRed(rgb), qGreen(rgb), qBlue(rgb), maskLine[k]);
            }
        }
    }
}

void ILayerControl::remaskImage(const QImage &mask){
    if(mask.depth() > 8) return;
    if(!modified){
        modifiedImage = image.copy();
    }
    modified = true;
    for(int i=0; i<modifiedImage.height(); i++){
        QRgb* line = (QRgb*)modifiedImage.scanLine(i);
        const uchar* maskLine = mask.constScanLine(i);
        for(int k=0; k<modifiedImage.width(); k++){
            QRgb rgb = line[k];
            line[k] = qRgba(qRed(rgb), qGreen(rgb), qBlue(rgb), maskLine[k]);
        }
    }
}

void ILayerControl::maskColor(const QColor &color){
    if(!modified){
        modifiedImage = image.copy();
    }
    modified = true;
    for(int i=0; i<modifiedImage.height(); i++){
        QRgb* line = (QRgb*)modifiedImage.scanLine(i);
        for(int k=0; k<modifiedImage.width(); k++){
            QRgb rgb = line[k];
            if(rgb == color.rgb()){
                line[k] = qRgba(qRed(rgb), qGreen(rgb), qBlue(rgb), 0);
            }
        }
    }
}

void ILayerControl::resetImage(){
    modified = false;
}
