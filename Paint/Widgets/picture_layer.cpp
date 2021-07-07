#include "picture_layer.h"

Picture_layer::Picture_layer()
{

}

Picture_layer::Picture_layer(QPixmap picture, QRect rect, QWidget* parent) : QWidget(parent)
{
    this->picture = picture;
    this->parent = parent;
    QPolygon polygon = QPolygon(rect);
    if(polygon.point(0) != polygon.point(polygon.size()-1))
    {
        polygon.append(polygon.point(0));
    }
    region = new Capture_region(this, parent, polygon);
    base_rect = region->get_polygon().boundingRect();
    region->move(150, 150);
    region->set_constraint(150, 150, 135+rect.width(), 135 + rect.height());
    parent->resize(rect.width()+400, rect.height()+400);
    parent->update();
}

Picture_layer::~Picture_layer()
{
    delete region;
}

void Picture_layer::paint(QImage& image)
{
    QPainter painter(&image);
    QRect rect = region->get_polygon().boundingRect();
    QPixmap temp = picture.copy(rect.x()-150, rect.y()-150, rect.width(), rect.height());
    painter.drawPixmap(rect.x(), rect.y(), temp);
    /*QPixmap temp = picture.copy();
    painter.drawPixmap(0, 0, temp);
    temp.fill(Qt::transparent);
    painter.setCompositionMode(QPainter::CompositionMode_Source);
    painter.drawPixmap(0, 0, picture);
    painter.setCompositionMode( QPainter::CompositionMode_DestinationIn );
    painter.setBackgroundMode(Qt::OpaqueMode);
    QPainterPath path;
    path.addPolygon(region->get_polygon());
    path = path.simplified();
    QPainterPath temp_path = QPainterPath();
    temp_path.addRect(base_rect);
    temp_path = temp_path.subtracted(path);
    painter.fillPath(temp_path, QColor(0, 0, 0, 0));*/
}

QPixmap* Picture_layer::get_pic()
{
    return &picture;
}

void Picture_layer::control_point_position_change(int index, QList<int> position, int dx, int dy)
{
    region->point_move(position, dx, dy);
}
