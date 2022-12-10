#include "mosicsample.h"
#include <QRandomGenerator>
#include <QPainter>

MosicSample::MosicSample(QWidget* parent) : QWidget(parent)
{
    image = QImage(42, 42, QImage::Format_ARGB32);
    for(int i=0; i<42; i++)
    {
        for(int k=0; k<42; k++)
        {
            quint32 value = QRandomGenerator::global()->generate();
            image.setPixel(i, k, value);
        }
    }
    mask = QImage(42, 42, QImage::Format_ARGB32);
    setMaximumSize(42, 42);
    setMinimumSize(42, 42);
    unit_size = 5;
    reset();
}

void MosicSample::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.drawImage(QPoint(0, 0), mask);
}

void MosicSample::reset()
{
    for(int i=0; i<42; i+=unit_size)
    {
        for(int k=0; k<42; k+=unit_size)
        {
            int red = 0;
            int blue = 0;
            int green = 0;
            int times = 0;
            for(int m=i; m<42 && m<image.width() && m<i+unit_size; m++)
            {
                for(int n=k; n<42 && n<image.height() && n<k+unit_size; n++)
                {
                    QRgb rgb = image.pixel(m, n);
                    red += qRed(rgb);
                    blue += qBlue(rgb);
                    green += qGreen(rgb);
                    times++;
                }
            }
            QColor color(red/times, blue/times, green/times);
            for(int m=i; m<42 && m<i+unit_size; m++)
            {
                for(int n=k; n<42 && n<k+unit_size; n++)
                {
                    mask.setPixelColor(m, n, color);
                }
            }
        }
    }
    update();
}

void MosicSample::setUnitSize(int unit_size)
{
    this->unit_size = unit_size;
    reset();
}

void MosicSample::setRange(int range)
{

}
