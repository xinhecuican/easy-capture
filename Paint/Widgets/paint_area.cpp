#include "paint_area.h"
#include "recorder.h"
#include "style_manager.h"
#include<QDir>
#include<QDateTime>
#include "history.h"

Paint_area::Paint_area()
{

}

Paint_area::~Paint_area()
{
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
    image = QImage(size(), QImage::Format_RGB32);
    now_path = QPainterPath();
    is_draw = false;
    is_eraser = false;
    pic_save = false;
    layer_num = 0;
}

Paint_area::Paint_area(QWidget* parent) : QWidget(parent)
{
    this->parent = parent;
    pic_layer = NULL;
    image = QImage(size(), QImage::Format_RGB32);
    layers = QVector<Ilayer*>();
    is_draw = false;
    now_path = QPainterPath();
    layer_num = 0;
}

void Paint_area::set_picture(QPixmap pixmap, QRect rect)
{
    pic_layer = new Picture_layer("第0层", pixmap, rect, this);
    layer_num++;
    layers.push_back(pic_layer);
}

void Paint_area::paintEvent(QPaintEvent *event)
{
    QColor backColor = qRgb(255,255,255);
    image.fill(backColor);
    QPainter painter(this);
    painter.setRenderHints(QPainter::SmoothPixmapTransform | QPainter::Antialiasing, true);
    for(int i=0; i<layers.size(); i++)
    {
        if(is_eraser)
        {
            layers[i]->erase_and_paint(point, image);
        }
        else
            layers[i]->paint(image);
    }
    painter.drawImage(0, 0, image);
    if(!is_eraser)
    {
        QPen pen;
        pen.setWidth(now_data.width);
        pen.setColor(now_data.color);
        pen.setCapStyle(now_data.cap_style);
        pen.setJoinStyle(now_data.join_style);
        painter.setPen(pen);
        painter.drawPath(now_path);
    }
}

void Paint_area::mouseMoveEvent(QMouseEvent *event)
{
    if(!is_draw)
    {
        return;
    }
    point = event->pos();
    now_path.lineTo(event->pos());
    update();
}

void Paint_area::mousePressEvent(QMouseEvent *event)
{
    if(event->button() == Qt::LeftButton)
    {
        is_draw = true;
    }
    point = event->pos();
    now_path = QPainterPath();
    now_path.moveTo(event->pos());
    now_data = Style_manager::instance()->get_now();
}

void Paint_area::mouseReleaseEvent(QMouseEvent* event)
{
    if(!is_eraser)
    {
        now_path.lineTo(event->pos());
        Paint_data* paint_data = Style_manager::instance()->get();
        int index = layers[layers.size()-1]->add_data(paint_data, now_path);
        Recorder::instance()->record(index, layers[layers.size()-1]);
        now_path = QPainterPath();
    }
    update();
}

void Paint_area::resizeEvent(QResizeEvent *event)
{
    image = QImage(event->size(), QImage::Format_RGB32);
}

void Paint_area::remove_layer(Paint_layer* layer)
{
    Recorder::instance()->remove_record(layer);
    layers.removeOne(layer);
    delete layer;
}

void Paint_area::remove_layer(int index)
{
    Recorder::instance()->remove_record(layers[index]);
    delete layers[index];
    layers.removeAt(index);
}

void Paint_area::raise_layer(int index)
{
    if(index >= layers.size() - 1)
    {
        return;
    }
    layers.move(index, index+1);
}

void Paint_area::drop_layer(int index)
{
    if(index <= 0)
    {
        return;
    }
    layers.move(index, index-1);
}

QString Paint_area::create_layer()
{
    QString name = "第" + QString::number(layer_num) + "层";
    layers.push_back(new Paint_layer(this, name));
    layer_num++;
    return name;
}

void Paint_area::using_erase(bool is_using_eraser)
{
    is_eraser = is_using_eraser;
}

QRect Paint_area::bounded_rect()
{
    QRect rect;
    for(int i=0; i<layers.size(); i++)
    {
        rect = rect.united(layers[i]->bounded_rect());
    }
    return rect;
}

void Paint_area::save(QString path)
{
    pic_save = true;
    QColor backColor = qRgb(255,255,255);
    image.fill(backColor);
    for(int i=0; i<layers.size(); i++)
    {
        layers[i]->paint(image);
    }
    image.copy(bounded_rect()).save(path);
    History::instance()->log(History_data::Persist, path);
}

void Paint_area::save_temp()
{
    QDir dir;
    if(!dir.exists("Data/Temp"))
    {
        dir.mkpath("Data/Temp");
    }
    QColor backColor = qRgb(255,255,255);
    image.fill(backColor);
    for(int i=0; i<layers.size(); i++)
    {
        layers[i]->paint(image);
    }
    QDateTime time = QDateTime::currentDateTime();
    QString path = "Data/Temp/" + time.toString("dd_mm_yyyy_hh_mm_ss") + "/";
    if(!dir.exists(path))
    {
        dir.mkpath(path);
    }
    path += "main.png";
    image = image.copy(bounded_rect());
    image.save(path);
    History::instance()->log(History_data::Editable, path);
}

bool Paint_area::contain_picture()
{
    return pic_layer != NULL;
}

QStringList Paint_area::layers_name()
{
    QStringList ans;
    for(int i=0; i<layers.size(); i++)
    {
        ans.append(layers[i]->get_name());
    }
    return ans;
}

void Paint_area::set_name(int index, QString text)
{
    layers[index]->set_name(text);
}
