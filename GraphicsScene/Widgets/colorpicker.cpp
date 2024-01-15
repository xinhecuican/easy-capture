#include "colorpicker.h"
#include <QRgb>
#include <QPainter>
#include "../../Helper/PaintHelper.h"

ColorPicker::ColorPicker(ILayerControl* manager, QGraphicsItem* parent) : QGraphicsObject(parent)
{
    this->manager = manager;
    screenGeometry = ImageHelper::getCurrentGeometry();
    int widthpx = pt2px(pixelWidthPT);
    pixelWidthPX = (widthpx | 0b1) * 5;
    int heightpx = pt2px(pixelHeightPT);
    pixelHeightPX = (heightpx | 0b1) * 5;
}

void ColorPicker::setPosition(QPointF point){
    this->point = point;
    if(point.x() + pixelWidthPX + offsetX > screenGeometry.right()){
        bound.setLeft(point.x()- pixelWidthPX - offsetX);
    }
    else{
        bound.setLeft(point.x() + offsetX);
    }
    if(point.y() + pixelHeightPX + 20 + offsetY > screenGeometry.bottom()){
        bound.setTop(point.y()- pixelHeightPX - 20 - offsetY);
    }
    else{
        bound.setTop(point.y() + offsetY);
    }
    bound.setHeight(pixelHeightPX+20);
    bound.setWidth(pixelWidthPX);
    update();
}

QRectF ColorPicker::boundingRect() const{
    //    QRectF ans = bound;
    //    ans.setTopLeft(ans.topLeft() - QPointF(shadowWidth, shadowWidth));
    //    ans.setBottomRight(ans.bottomRight() + QPointF(shadowWidth, shadowWidth));
    return bound;
}

QPainterPath ColorPicker::shape() const{
    QPainterPath path;
    //    QRectF ans = bound;
    //    ans.setTopLeft(ans.topLeft() - QPointF(shadowWidth, shadowWidth));
    //    ans.setBottomRight(ans.bottomRight() + QPointF(shadowWidth, shadowWidth));
    //    path.addRect(ans);
    path.addRect(bound);
    return path;
}

void ColorPicker::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget){
    if(!manager->isImageValid()) return;
    QImage image = manager->getImage();
    QImage image1 = QImage(pixelWidthPX, pixelHeightPX,QImage::Format_RGB32);
    for(int i=0; i<pixelWidthPX/5; i++) {
        for(int k=0; k<pixelHeightPX/5; k++) {
            for(int j=i*5; j<i*5+5; j++) {
                for(int m=k*5; m<k*5+5; m++) {
                    int x = i+point.x()-pixelWidthPX/10 < 0 ? 0 : i+point.x()+pixelWidthPX/10>=image.width() ? image.width()-1 : i+point.x() - pixelWidthPX/10;
                    int y = k+point.y()-pixelHeightPX/10 < 0 ? 0 : k+point.y()+pixelHeightPX/10>=image.height() ? image.height()-1 : k+point.y()-pixelHeightPX/10;
                    QRgb* line = (QRgb*)image1.scanLine(m);
                    line[j] = ((QRgb*)image.constScanLine(y))[x];
//                    image1.setPixel(j, m, image.pixel(x, y));
                }
            }
        }
    }
    QRgb rgb = image.pixel(point.x(), point.y());
    painter->drawImage(bound.left(), bound.top(), image1);
    painter->fillRect(bound.left(), bound.top()+pixelHeightPX, 20, 20, QColor(rgb));
    QRect textBound(bound.left()+20, bound.top()+pixelHeightPX, pixelWidthPX-20, 20);
    painter->fillRect(textBound, QColor(255, 255, 255));
    painter->drawText(textBound, Qt::AlignCenter,  "("+QString::number(qRed(rgb)) + "," + QString::number(qGreen(rgb)) + "," + QString::number(qBlue(rgb)) + ")");
//    PaintHelper::paintShadow(painter, bound, shadowWidth, QColor(50, 50, 50, 120));
    painter->setCompositionMode(QPainter::RasterOp_SourceOrNotDestination);
    double middlex = (double)(pixelWidthPX) / 2.0;
    double middley = (double)(pixelHeightPX) / 2.0;
    painter->drawLine(bound.left()+middlex-10, bound.top()+middley, bound.left()+middlex+10, bound.top()+middley);
    painter->drawLine(bound.left()+middlex, bound.top()+middley-10, bound.left()+middlex, bound.top()+middley+10);
}
