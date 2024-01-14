#include "blurlayer.h"
#include <QtMath>
#include "../../Helper/imagehelper.h"
#include <opencv2/opencv.hpp>
using namespace cv;

BlurLayer::BlurLayer(const QString& name,
                     ILayerControl* manager,
                     QGraphicsItem* parent)
    : LayerBase(name, manager, parent),
      range(4),
      windowSize(20)
{
    if(manager->isImageValid()){
        use = QBitArray(manager->getOriginImage().height() * manager->getOriginImage().width());
    }
}

void BlurLayer::onImageSet() {
    use.fill(false);
    use.resize(manager->getOriginImage().height() * manager->getOriginImage().width());
}

QRectF BlurLayer::boundingRect()const {
    return bound;
}

void BlurLayer::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) {
}

void BlurLayer::addPoint(const QPoint &point) {
    auto image = manager->getOriginImage();
    QPoint leftTop = point - QPoint(windowSize, windowSize);
    if(leftTop.x() < 0) leftTop.setX(0);
    if(leftTop.y() < 0) leftTop.setY(0);
    QPoint rightBotttom = point + QPoint(windowSize, windowSize);
    if(rightBotttom.x() >= image.width()) rightBotttom.setX(image.width() - 1);
    if(rightBotttom.y() >= image.height()) rightBotttom.setY(image.height() - 1);
    QRect boundRect(leftTop, rightBotttom);
    if(boundRect.width() <= 0 || boundRect.height() <= 0) return;
    bound = bound.united(boundRect);

    QImage maskImage = image.copy(boundRect);
    Mat maskMat = ImageHelper::QImage2Mat(maskImage);
    Mat dst;
    cv::GaussianBlur(maskMat, dst, Size(11, 11), 11);
    maskImage = ImageHelper::Mat2QImage(dst);
    for(int i=leftTop.x(); i<rightBotttom.x(); i++){
        for(int k=leftTop.y(); k<rightBotttom.y(); k++){
            int pos = i * image.height() + k;
            if(!use[pos]) {
                use.setBit(pos);
                manager->modifyImage(QPoint(i, k), maskImage.pixel(i-leftTop.x(),k-leftTop.y()));
            }
        }
    }
    update();
}

void BlurLayer::deletePoint(const QPoint &point){
    auto image = manager->getOriginImage();
    QPoint leftTop = point - QPoint(windowSize, windowSize);
    if(leftTop.x() < 0) leftTop.setX(0);
    if(leftTop.y() < 0) leftTop.setY(0);
    QPoint rightBotttom = point + QPoint(windowSize, windowSize);
    if(rightBotttom.x() >= image.width()) rightBotttom.setX(image.width() - 1);
    if(rightBotttom.y() >= image.height()) rightBotttom.setY(image.height() - 1);
    QRectF boundRect(leftTop, rightBotttom);
    if(boundRect.width() <= 0 || boundRect.height() <= 0) return;
    for(int i=leftTop.x(); i<rightBotttom.x(); i++){
        for(int k=leftTop.y(); k<rightBotttom.y(); k++){
            int pos = i * image.height() + k;
            use.clearBit(pos);
            manager->modifyImage(QPoint(i, k), image.pixel(i, k));
        }
    }
    update();
}

void BlurLayer::reset() {
    use.resize(0);
    LayerBase::reset();
}

void BlurLayer::setParameter(const QString &name, const QVariant &var) {
    if(name == "range") {
        range = var.toInt();
    }
    if(name == "windowSize") {
        windowSize = var.toInt();
    }
}

void BlurLayer::setEnable(bool enable, int index) {
    this->enable = false;
}

int BlurLayer::type() const{
    return 102401;
}

QRectF BlurLayer::getSaveRect(){
    return QRectF();
}

void BlurLayer::onDelete(const QPointF &point){
    deletePoint(point.toPoint());
}
