#include "paint_area.h"
#include "recoder.h"
#include "style_manager.h"

Paint_area::Paint_area()
{

}

Paint_area::~Paint_area()
{
    if(pic_layer != NULL)
    {
        delete pic_layer;
    }
    for(int i=0; i<layers.size(); i++)
    {
        delete layers[i];
    }
}

void Paint_area::reset()
{
    for(int i=0; i<layers.size(); i++)
    {
        delete layers[i];
    }
    layers.clear();
    now_path = QPainterPath();
    is_draw = false;
    layer_num = 0;
    create_layer();
}

Paint_area::Paint_area(QWidget* parent) : QWidget(parent)
{
    this->parent = parent;
    pic_layer = NULL;
    image = QImage(size(), QImage::Format_RGB32);
    layers = QVector<Paint_layer*>();
    is_draw = false;
    now_path = QPainterPath();
    layer_num = 0;
    create_layer();
}

void Paint_area::set_picture(QPixmap pixmap, QRect rect)
{
    if(pic_layer != NULL)
    {
        delete pic_layer;
    }
    pic_layer = new Picture_layer(pixmap, rect, this);
}

void Paint_area::paintEvent(QPaintEvent *event)
{
    QColor backColor = qRgb(255,255,255);
    image.fill(backColor);
    QPainter painter(this);
    pic_layer->paint(image);
    painter.drawImage(0, 0, image);
    for(int i=0; i<layers.size(); i++)
    {
        layers[i]->paint(image);
    }
    painter.drawImage(0, 0, image);
    painter.setRenderHints(QPainter::SmoothPixmapTransform | QPainter::Antialiasing, true);
    QPen pen;
    pen.setWidth(now_data.width);
    pen.setColor(now_data.color);
    painter.setPen(pen);
    painter.drawPath(now_path);
}

void Paint_area::mouseMoveEvent(QMouseEvent *event)
{
    if(!is_draw)
    {
        return;
    }
    now_path.lineTo(event->pos());
    update();
}

void Paint_area::mousePressEvent(QMouseEvent *event)
{
    if(event->button() == Qt::LeftButton)
    {
        is_draw = true;
    }
    now_path = QPainterPath();
    now_path.moveTo(event->pos());
    now_data = Style_manager::instance()->get_now();
}

void Paint_area::mouseReleaseEvent(QMouseEvent* event)
{
    now_path.lineTo(event->pos());
    Paint_data* paint_data = Style_manager::instance()->get();
    int index = layers[layers.size()-1]->add_data(paint_data, now_path);
    Recoder::instance()->record(index, layers[layers.size()-1]);
    now_path = QPainterPath();
    update();
}

void Paint_area::resizeEvent(QResizeEvent *event)
{
    image = QImage(event->size(), QImage::Format_RGB32);
}

void Paint_area::remove_layer(Paint_layer* layer)
{
    layers.removeOne(layer);
    delete layer;
}

void Paint_area::remove_layer(int index)
{
    Recoder::instance()->remove_record(layers[index]);
    delete layers[index];
    layers.removeAt(index);
}

void Paint_area::raise_layer(Paint_layer* layer)
{
    int index = layers.indexOf(layer);
    if(index == layers.size() - 1)
    {
        return;
    }
    layers.move(index, index+1);
}

void Paint_area::drop_layer(Paint_layer* layer)
{
    int index = layers.indexOf(layer);
    if(index == 0)
    {
        return;
    }
    layers.move(index, index-1);
}

void Paint_area::create_layer()
{
    layers.push_back(new Paint_layer(this, "第" + (QString)layer_num + "层"));
    layer_num++;
}
