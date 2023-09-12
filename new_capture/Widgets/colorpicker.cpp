#include "colorpicker.h"
#include <QRgb>
#include <QPainter>

ColorPicker::ColorPicker(QGraphicsItem* parent) : QGraphicsObject(parent)
{
    screenGeometry = ImageHelper::getCurrentGeometry();
}

void ColorPicker::setPic(const QPixmap &pix){
    this->image = pix.toImage();
}

void ColorPicker::setPosition(QPointF point){
    this->point = point;
    if(point.x() + 145 > screenGeometry.right()){
        bound.setLeft(point.x()-145);
    }
    else{
        bound.setLeft(point.x());
    }
    if(point.y() + 85 > screenGeometry.bottom()){
        bound.setTop(point.y()-85);
    }
    else{
        bound.setTop(point.y());
    }
    bound.setHeight(105);
    bound.setWidth(145);
}

QRectF ColorPicker::boundingRect() const{
    return bound;
}

QPainterPath ColorPicker::shape() const{
    QPainterPath path;
    path.addRect(bound);
    return path;
}

void ColorPicker::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget){
    QImage image1 = QImage(145, 85,QImage::Format_RGB32);
    for(int i=0; i<29; i++) {
        for(int k=0; k<17; k++) {
            for(int j=i*5; j<i*5+5; j++) {
                for(int m=k*5; m<k*5+5; m++) {
                    int x = i+point.x()-14 < 0 ? 0 : i+point.x()+14>=image.width() ? image.width()-1 : i+point.x() - 14;
                    int y = k+point.y()-8 < 0 ? 0 : k+point.y()+8>=image.height() ? image.height()-1 : k+point.y()-8;
                    image1.setPixel(j, m, image.pixel(x, y));
                }
            }
        }
    }
    QImage image2 = QImage(20, 20, QImage::Format_RGB32);
    QRgb rgb = image.pixel(point.x(), point.y());
    image2.fill(rgb);
    painter->drawImage(bound.left(), bound.top(), image1);
    painter->drawImage(bound.left(), bound.top()+85, image2);
    QRect textBound(bound.left()+20, bound.top()+85, 126, 20);
    painter->fillRect(textBound, QColor(255, 255, 255));
    painter->drawText(textBound, Qt::AlignCenter,  "("+QString::number(qRed(rgb)) + "," + QString::number(qGreen(rgb)) + "," + QString::number(qBlue(rgb)) + ")");
    painter->setCompositionMode(QPainter::RasterOp_SourceOrNotDestination);
    painter->drawLine(bound.left()+65, bound.top()+42.5, bound.left()+80, bound.top()+42.5);
    painter->drawLine(bound.left()+72.5, bound.top()+35, bound.left()+72.5, bound.top()+50);
}
