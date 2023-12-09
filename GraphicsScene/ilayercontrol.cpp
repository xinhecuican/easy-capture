#include "ilayercontrol.h"

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
    modifiedImage.setPixel(pos, color.rgba());
}

bool ILayerControl::isImageValid(){
    return imageValid;
}

void ILayerControl::maskImage(const QImage &mask){
    if(!modified){
        modifiedImage = image.copy();
    }
    modified = true;
    modifiedImage.setAlphaChannel(mask);
}
