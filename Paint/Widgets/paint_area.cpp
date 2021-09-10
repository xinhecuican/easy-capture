#include "paint_area.h"
#include "recorder.h"
#include "style_manager.h"
#include<QDir>
#include<QDateTime>
#include "history.h"
#include<QTimer>
#include "Helper/debug.h"
#include<QFileDialog>
#include<QMouseEvent>
#include "opencv2/core.hpp"
#include "opencv2/opencv.hpp"
#include "Helper/image_helper.h"
#include<windows.h>
#include<QImageWriter>
#include "Helper/image_helper.h"
#include<QPixmapCache>

Paint_area::Paint_area()
{

}

Paint_area::~Paint_area()
{
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
    is_eraser = false;
    pic_save = false;
    layer_num = 0;
}

Paint_area::Paint_area(QWidget* parent) : QWidget(parent)
{
    this->parent = parent;
    pic_layer = NULL;
    layers = QVector<Ilayer*>();
    is_draw = false;
    now_path = QPainterPath();
    layer_num = 0;
}

void Paint_area::set_picture(QPixmap pixmap, QRect rect)
{
    is_save = false;
    pic_layer = new Picture_layer("第0层", pixmap, rect, this);
    layer_num++;
    layers.push_back(pic_layer);
    QTimer::singleShot(100, this, [=](){
        this->update();
    });
}

void Paint_area::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);

    painter.setRenderHints(QPainter::SmoothPixmapTransform | QPainter::Antialiasing, true);
    paint(&painter, disable_color);
    //painter.drawImage(0, 0, image);
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

void Paint_area::paint(QPainter* painter, QList<QColor> disable_color)
{
    for(int i=0; i<layers.size(); i++)//各层绘制
    {
        if(is_eraser && is_draw)
        {
            layers[i]->erase_and_paint(point, painter, disable_color);
        }
        else
        {
            layers[i]->paint(painter, disable_color, is_save);
        }
    }

}

void Paint_area::paint_rect(QRect rect)
{
    if(image.isNull())
    {
        return;
    }
    if(is_eraser)
    {
        QColor backColor = qRgb(255,255,255);
        image.fill(backColor);
        for(int i=0; i<layers.size(); i++)//各层绘制
        {
            //layers[i]->erase_and_paint(point, image, is_save);
        }
    }
    QPainter painter(&image);
    for(int i=0; i<disable_color.size(); i++)//设置透明色
    {
        QImage mask = image.createMaskFromColor(disable_color[i].rgb(), Qt::MaskOutColor);
        image.setAlphaChannel(mask);
    }
    if(is_eraser)
    {
        update();
    }
    else
    {
        update(rect);
    }
}

void Paint_area::mouseMoveEvent(QMouseEvent *event)
{
    if(!is_draw)
    {
        return;
    }
    point = event->pos();
    QPainterPath::Element ele = now_path.elementAt(now_path.elementCount()-1);
    now_path.lineTo(event->pos());
    int x, y, w, h;
    QPoint global_point = event->pos();
    if(ele.x < global_point.x())
    {
        x = ele.x;
        w = global_point.x() - ele.x;
    }
    else
    {
        x = global_point.x();
        w = ele.x - global_point.x();
    }
    if(ele.y < global_point.y())
    {
        y = ele.y;
        h = global_point.y() - ele.y;
    }
    else
    {
        y = global_point.y();
        h = ele.y - global_point.y();
    }
    x -= now_data.width+3;
    y -= now_data.width+3;
    w += now_data.width+6;
    h += now_data.width+6;
    update(QRect(x, y, w, h));
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
    }
    is_draw = false;
    now_path = QPainterPath();
    update();
}

void Paint_area::resizeEvent(QResizeEvent *event)
{
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
    if(path == "")
    {
        return;
    }
    is_save = true;
    repaint();
    is_save = false;
    pic_save = true;
    QRect rect = bounded_rect();
    cv::Mat ans(rect.height(), rect.width(), CV_8UC4);
    if(rect.height() >= 32700)//图片过大需要切分
    {
        for(int i=0; i<rect.height(); i+=32700)
        {
            int height = rect.height() - i > 32700 ? 32700 : rect.height() - i;
            QRect temp_rect(rect.left(), rect.top()+i, rect.width(), height);
            QImage temp = grab(temp_rect).toImage();
            for(int i=0; i<disable_color.size(); i++)//设置透明色
            {
                QImage mask = temp.createMaskFromColor(disable_color[i].rgb(), Qt::MaskOutColor);
                temp.setAlphaChannel(mask);
            }
            cv::Mat temp_mat = Image_helper::QImage2Mat(temp);
            temp_mat.copyTo(ans(cv::Rect(0, i, rect.width(), height)));
        }
    }
    else
    {
        QImage temp =  grab(bounded_rect()).toImage();
        for(int i=0; i<disable_color.size(); i++)//设置透明色
        {
            QImage mask = temp.createMaskFromColor(disable_color[i].rgb(), Qt::MaskOutColor);
            temp.setAlphaChannel(mask);
        }

        ans = Image_helper::QImage2Mat(temp);
    }
    cv::imwrite(std::string(path.toLocal8Bit()), ans);
    History::instance()->log(History_data::Persist, path);
}

void Paint_area::save_temp()
{
    is_save = true;
    repaint();
    is_save = false;
    QDir dir;
    if(!dir.exists("Data/Temp"))
    {
        dir.mkpath("Data/Temp");
    }
    //paint(true);
    QDateTime time = QDateTime::currentDateTime();
    QString path = "Data/Temp/" + time.toString("dd_mm_yyyy_hh_mm_ss") + "/";
    if(!dir.exists(path))
    {
        dir.mkpath(path);
    }
    path += "main.png";
    QRect rect = bounded_rect();
    if(rect.height() >= 32767)
    {
        return;
    }
    else
    {
        QImage temp = grab(rect).toImage();
        for(int i=0; i<disable_color.size(); i++)//设置透明色
        {
            QImage mask = temp.createMaskFromColor(disable_color[i].rgb(), Qt::MaskOutColor);
            temp.setAlphaChannel(mask);
        }
        temp.save(path);
    }
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

void Paint_area::set_disable_color(int index, QColor color)
{
    if(index == -1)
    {
        disable_color.append(color);
    }
    else
    {
        disable_color.removeAt(index);
    }
    update();
}

void Paint_area::update_image(QRect bound_rect)
{
    image_bound = bound_rect;
    image = QImage(bound_rect.width(), bound_rect.height(), QImage::Format_RGB32);
}

QImage Paint_area::get_image()
{
    QImage temp = grab(bounded_rect()).toImage();
    for(int i=0; i<disable_color.size(); i++)//设置透明色
    {
        QImage mask = temp.createMaskFromColor(disable_color[i].rgb(), Qt::MaskOutColor);
        temp.setAlphaChannel(mask);
    }
    return temp;
}
