#include "blurlayer.h"
#include<QRgb>
#include<QPainter>

BlurLayer::BlurLayer(QGraphicsItem* parent) : QGraphicsObject(parent), is_setpic(false)
{
    is_allocate = false;
    is_save = false;
}

void BlurLayer::setPix(const QPixmap& pix, QPoint pos)
{
    if(is_allocate)
    {
        for(int i=0; i<pix.height() / range + 1; i++)
        {
            delete [] mask_use[i];
        }
        delete [] mask_use;
    }

    this->pix = pix.toImage();
    left_top = pos;
    setPos(pos);
    mask = QImage(pix.width(), pix.height(), QImage::Format_ARGB32);
    mask.fill(Qt::transparent);
    resetMaskSetting();
    this->is_setpic = true;
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

    for(int i=begin_x; i<begin_x+range && i<pix.width(); i+=unit_size)
    {
        for(int k=begin_y; k<begin_y+range && k<pix.height(); k+=unit_size)
        {
            red = 0;
            blue = 0;
            green = 0;
            times = 0;
            for(int m=i; m<begin_x+range && m<pix.width() && m<i+unit_size; m++)
            {
                for(int n=k; n<begin_y+range && n<pix.height() && n<k+unit_size; n++)
                {
                    QRgb rgb = pix.pixel(m, n);
                    red += qRed(rgb);
                    blue += qBlue(rgb);
                    green += qGreen(rgb);
                    times++;
                }
            }
            QColor color(red/times, green/times, blue/times);
            for(int m=i; m<begin_x+range && m<pix.width() && m<i+unit_size; m++)
            {
                for(int n=k; n<begin_y+range && n<pix.height() && n<k+unit_size; n++)
                {
                    mask.setPixelColor(m, n, color);
                }
            }
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
//    if(!is_save)
        painter->drawImage(QPointF(0, 0), mask);
}

bool BlurLayer::isSetPic()
{
    return is_setpic;
}

void BlurLayer::showNormal()
{
    is_save = false;
}

void BlurLayer::hideNormal()
{
    is_save = true;
}

void BlurLayer::setRange(int range)
{
    this->range = range;
    resetMaskSetting();
}

void BlurLayer::setUnitSize(int unit_size)
{
    this->unit_size = unit_size;
}

void BlurLayer::resetMaskSetting()
{
    if(is_allocate)
    {
        for(int i=0; i<pix.height() / range + 1; i++)
        {
            delete [] mask_use[i];
        }
        delete [] mask_use;
    }
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
    is_allocate = true;
}

bool BlurLayer::acceptFocus()
{
    return false;
}

void BlurLayer::reset()
{
    if(is_allocate)
    {
        for(int i=0; i<pix.height() / range + 1; i++)
        {
            delete [] mask_use[i];
        }
        delete [] mask_use;
    }
    is_allocate = false;
    is_save = false;
    is_setpic = false;
}

int BlurLayer::type() const
{
    return 65538;
}
