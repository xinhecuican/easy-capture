#include "blurlayer.h"
#include<QRgb>
#include<QPainter>
#include "Manager/uimanager.h"

BlurLayer::BlurLayer(QGraphicsItem* parent) : QGraphicsObject(parent), is_setpic(false) {
    is_allocate = false;
    is_save = false;
    unit_size = UIManager::instance()->getMosaicUnitSize();
    range = UIManager::instance()->getMosaicRange();
}

void BlurLayer::setPix(const QPixmap& pix, QPoint pos) {
    this->pix = pix.toImage();
    left_top = pos;
    setPos(pos);
    mask = QImage(pix.width(), pix.height(), QImage::Format_ARGB32);
    mask.fill(Qt::transparent);
    resetMaskSetting();
    this->is_setpic = true;
}

BlurLayer::~BlurLayer() {
    clearMaskUse();
}

QRectF BlurLayer::boundingRect()const {
    return QRectF(QPointF(0, 0), mask.size());
}

QPainterPath BlurLayer::shape() const {
    QPainterPath ans;
    ans.addRect(QRectF(QPointF(0, 0), mask.size()));
    return ans;
}

void BlurLayer::addPoint(QPoint point) {
    point = QPoint(point.x()-left_top.x(), point.y()-left_top.y());
    if(!is_allocate || point.x() < 0 || point.y() < 0 || point.x() > pix.width() || point.y() > pix.height())
        return;
    int red = 0;
    int blue = 0;
    int green = 0;
    int times = 0;
    int begin_x = point.x() - range - point.x() % maskUnitSize;
    int begin_y = point.y() - range - point.y() % maskUnitSize;

    bool allUse = true;
    int unitDelta = range / maskUnitSize;
    int maskUsex = begin_x / maskUnitSize;
    int maskUsey = begin_y / maskUnitSize;
    for(int m=maskUsex; m<maskUsex + 2 * unitDelta && m<maskUseWidth; m++) {
        for(int n=maskUsey; n<maskUsey + 2 * unitDelta && n<maskUseHeight; n++) {
            if(m >= 0 && n >= 0 && !mask_use[m][n]) {
                allUse = false;
                break;
            }
        }
    }
    if(!allUse) {
        for(int i=begin_x; i<begin_x + 2 * range && i<pix.width(); i+=unit_size) {
            for(int k=begin_y; k<begin_y + 2 * range && k<pix.height(); k+=unit_size) {

                red = 0;
                blue = 0;
                green = 0;
                times = 0;
                for(int m=i; m<begin_x + 2 * range && m<pix.width() && m<i+unit_size; m++) {
                    for(int n=k; n<begin_y + 2 * range && n<pix.height() && n<k+unit_size; n++) {
                        if(m >= 0 && n >= 0) {
                            QRgb rgb = pix.pixel(m, n);
                            red += qRed(rgb);
                            blue += qBlue(rgb);
                            green += qGreen(rgb);
                            times++;
                        }
                    }
                }
                if(times != 0) {
                    QColor color(red/times, green/times, blue/times);
                    for(int m=i; m<begin_x + 2 * range && m<pix.width() && m<i+unit_size; m++) {
                        for(int n=k; n<begin_y + 2 * range && n<pix.height() && n<k+unit_size; n++) {
                            if(m >= 0 && n >= 0 && m/maskUnitSize < maskUseWidth && n/maskUnitSize < maskUseHeight && !mask_use[m/maskUnitSize][n/maskUnitSize])
                                mask.setPixelColor(m, n, color);
                        }
                    }
                }
            }
        }

        for(int m=maskUsex; m<maskUsex + 2 * unitDelta && m<maskUseWidth; m++) {
            for(int n=maskUsey; n<maskUsey + 2 * unitDelta && n<maskUseHeight; n++) {
                if(m >= 0 && n >= 0)
                    mask_use[m][n] = true;
            }
        }
    }
    update();
}

void BlurLayer::deletePoint(QPoint point) {
    point = QPoint(point.x()-left_top.x(), point.y()-left_top.y());
    if(is_allocate) {
        int begin_x = point.x() - range - point.x() % range;
        int begin_y = point.y() - range - point.y() % range;
        bool hasUse = false;
        int unitDelta = range / maskUnitSize;
        int maskUsex = begin_x / maskUnitSize;
        int maskUsey = begin_y / maskUnitSize;
        for(int m=maskUsex; m<maskUsex + 2 * unitDelta && m<maskUseWidth; m++) {
            for(int n=maskUsey; n<maskUsey + 2 * unitDelta & n<maskUseHeight; n++) {
                if(m >= 0 && n >= 0 && mask_use[m][n]) {
                    hasUse = true;
                    mask_use[m][n] = false;
                }
            }
        }
        if(hasUse) {
            for(int i=begin_x; i<begin_x+2*range && i<pix.width(); i++) {
                for(int k=begin_y; k<begin_y+2*range && k<pix.height(); k++) {
                    if(i >= 0 && k >= 0)
                        mask.setPixelColor(i, k, QColor(0, 0, 0, 0));
                }
            }
            update();
        }
    }
}

void BlurLayer::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) {
//    if(!is_save)
    painter->drawImage(QPointF(0, 0), mask);
}

bool BlurLayer::isSetPic() {
    return is_setpic;
}

void BlurLayer::showNormal() {
    is_save = false;
}

void BlurLayer::hideNormal() {
    is_save = true;
}

void BlurLayer::setRange(int range) {
    this->range = range - range % maskUnitSize;
}

void BlurLayer::setUnitSize(int unit_size) {
    this->unit_size = unit_size - unit_size % maskUnitSize;
}

void BlurLayer::resetMaskSetting() {
    clearMaskUse();
    mask_use = new bool*[pix.width() / maskUnitSize + 1];
    for(int i=0; i<pix.width() / maskUnitSize + 1; i++) {
        mask_use[i] = new bool[pix.height() / maskUnitSize + 1];
    }
    for(int i=0; i<pix.width() / maskUnitSize + 1; i++) {
        for(int k=0; k<pix.height() / maskUnitSize + 1; k++) {
            mask_use[i][k] = false;
        }
    }
    maskUseWidth = pix.width() / maskUnitSize + 1;
    maskUseHeight = pix.height() / maskUnitSize + 1;
    is_allocate = true;
}

bool BlurLayer::acceptFocus() {
    return false;
}

void BlurLayer::reset() {
    clearMaskUse();
    is_save = false;
    is_setpic = false;
}

int BlurLayer::type() const {
    return 65538;
}

void BlurLayer::clearMaskUse() {
    if(is_allocate) {
        for(int i=0; i<pix.width() / maskUnitSize + 1; i++) {
            delete [] mask_use[i];
        }
        delete [] mask_use;
        is_allocate = false;
    }
}
