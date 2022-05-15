#include "blurlayer.h"
#include<QRgb>
#include<QPainter>

BlurLayer::BlurLayer(QGraphicsItem* parent) : QGraphicsObject(parent), is_setpic(false)
{
    is_allocate = false;
}

void BlurLayer::setPix(const QPixmap& pix)
{
    if(is_allocate)
    {
        for(int i=0; i<pix.width() / range + 1; i++)
        {
            delete [] mask_use[i];
        }
        delete [] mask_use;
    }

    this->pix = pix.toImage();
    left_top = QPoint(pix.width() / 2, pix.height() / 2);
    setPos(left_top);
    mask = QImage(pix.width(), pix.height(), QImage::Format_ARGB32);
    mask.fill(Qt::transparent);
    mask_use = new bool*[pix.height() / range + 1];
    for(int i=0; i<pix.height() / range + 1; i++)
    {
        mask_use[i] = new bool[pix.width() / range + 1];
    }
    for(int i=0; i<pix.height() / range + 1; i++)
    {
        for(int k=0; k<pix.width() / range + 1; k++)
        {
            mask_use[i][k] = false;
        }
    }
    this->is_setpic = true;
    is_allocate = true;
}

BlurLayer::~BlurLayer()
{
    if(is_allocate)
    {
        for(int i=0; i<pix.height() / range + 1; i++)
        {
            delete [] mask_use[i];
        }
        delete [] mask_use;
    }
}

QRectF BlurLayer::boundingRect()const
{
    return QRectF(QPointF(0, 0), mask.size());
}

QPainterPath BlurLayer::shape() const
{
    QPainterPath ans;
    ans.addRect(QRectF(QPointF(0, 0), mask.size()));
    return ans;
}

void BlurLayer::addPoint(QPoint point)
{
    point = QPoint(point.x()-left_top.x(), point.y()-left_top.y());
    if(!is_allocate || point.x() < 0 || point.y() < 0 || point.x() > pix.width() || point.y() > pix.height())
        return;
    if(mask_use[(int)point.y() / range][(int)point.x() / range])
    {
        return;
    }
    mask_use[(int)point.y() / range][(int)point.x() / range] = true;
    int red = 0;
    int blue = 0;
    int green = 0;
    int times = 0;
    int begin_x = point.x() - point.x() % range;
    int begin_y = point.y() - point.y() % range;
    for(int i=begin_x; i<begin_x+range && i<pix.width(); i++)
    {
        for(int k=begin_y; k<begin_y+range && k<pix.height(); k++)
        {
            QRgb rgb = pix.pixel(i, k);
            red += qRed(rgb);
            blue += qBlue(rgb);
            green += qGreen(rgb);
            times++;
        }
    }
    QColor color(red/times, blue/times, green/times);
    for(int i=begin_x; i<begin_x+range && i<pix.width(); i++)
    {
        for(int k=begin_y; k<begin_y+range && k<pix.height(); k++)
        {
            mask.setPixelColor(i, k, color);
        }
    }
    update();
}

void BlurLayer::deletePoint(QPoint point)
{
    point = QPoint(point.x()-left_top.x(), point.y()-left_top.y());
    if(is_allocate && mask_use[point.y()/range][point.x()/range])
    {
        int begin_x = point.x() - point.x()%range;
        int begin_y = point.y() - point.y() % range;
        for(int i=begin_x; i<begin_x+range && i<pix.width(); i++)
        {
            for(int k=begin_y; k<begin_y+range && k<pix.height(); k++)
            {
                mask.setPixelColor(i, k, QColor(0, 0, 0, 0));
            }
        }
        update();
        mask_use[point.y()/range][point.x()/range] = false;
    }
}

void BlurLayer::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    painter->drawImage(QPointF(0, 0), mask);
}

bool BlurLayer::isSetPic()
{
    return this->is_setpic;
}

void BlurLayer::showNormal()
{

}

void BlurLayer::hideNormal()
{

}
