#include "capture_region.h"
#include "Helper/stackdumper.h"

Capture_region::Capture_region()
{
    polygon = QPolygon();
    points = PList<Stretch_point*>();
}

Capture_region::Capture_region(QWidget* parent)
{
    this->parent = parent;
    polygon = QPolygon();
    points = PList<Stretch_point*>();
    free_points = PList<Stretch_point*>();
}

Capture_region::Capture_region(Ipoint_position_change* region, QWidget* parent)
{
    this->parent = parent;
    this->region = region;
    polygon = QPolygon();
    points = PList<Stretch_point*>();
    free_points = PList<Stretch_point*>();
}

Capture_region::Capture_region(Ipoint_position_change* region, QWidget* parent, QRect rect)
{
    this->parent = parent;
    this->region = region;
    polygon = QPolygon();
    points = PList<Stretch_point*>();
    free_points = PList<Stretch_point*>();
    unit(rect);
}

void Capture_region::clear()
{
    polygon.clear();
    points.clear_all();
    free_points.clear_all();
}

Capture_region::~Capture_region()
{

}

void Capture_region::unit(Capture_region &region)
{
    polygon = polygon.united(region.polygon);
    set_nodes();
}

void Capture_region::unit(QRect &rect)
{
    QPolygon temp_polygon = QPolygon();
    int w = rect.width();
    int h = rect.height();
    int x = rect.x();
    int y = rect.y();
    temp_polygon.setPoints(5, x, y, x+w, y, x+w, y+h, x, y+h, x, y);
    polygon = polygon.united(temp_polygon);
    set_nodes();
}

bool Capture_region::intersected(Capture_region &region)
{
    return !this->polygon.intersected(region.polygon).isEmpty();
}

bool Capture_region::intersected(QRect &rect)
{
    return !this->polygon.intersected(rect).isEmpty();
}

void Capture_region::move(int dx, int dy)
{
    polygon.translate(dx, dy);
    for(int i=0; i<points.size(); i++)
    {
        points[i]->translate(dx, dy);
    }
}

void Capture_region::point_move(QList<int> position, int dx, int dy)
{
    for(int i=0; i<position.size(); i++)
    {
        polygon[position[i]].setX(polygon[position[i]].x() + dx);
        polygon[position[i]].setY(polygon[position[i]].y() + dy);
    }
}

bool Capture_region::contains(QPoint point)
{
    return polygon.containsPoint(point, Qt::OddEvenFill);
}

QPolygon Capture_region::get_polygon()
{
    return polygon;
}

void Capture_region::set_nodes()
{
    QList<QPoint> list = polygon.toList();
    if(list.size() == 0)
    {
        return;
    }
    int len1 = list.size();
    int points_len = 1;


    QPoint point = list[0];
    create_point(0);
    points[0]->set_node(point);
    points[0]->show();
    points[0]->set_index(0);
    Stretch_point* now = points[0];

    for(int i=1; i<len1; i++)
    {
        if(point == list[i])
        {
            now->set_neigh(points[i-1]);
            points[i-1]->set_neigh(now);
            now->set_neigh(points[i-1]);
            now->set_index(i);
            if(i+1 < len1)
            {
                create_point(points_len);
                now = points[points_len];
                now->set_node(QPoint(-1, -1));
                points_len++;
                if(i+2 >= len1)
                {
                    if(list[i] != points[0]->get_node())
                    {
                        points[0]->set_index(i+1);
                        break;
                    }
                }
            }
        }
        else
        {
            create_point(points_len);
            points[points_len]->show();
            points[points_len]->set_node(list[i]);
            points[points_len]->set_neigh(points[points_len-1]);
            points[points_len]->set_index(i);
            points[points_len-1]->set_neigh(points[points_len]);
            points_len++;
        }
    }
    //多余节点转换为空闲节点
    for(int i=points_len; i<points.size(); i++)
    {
        free_points.append(points[i]);
        points[i]->hide();
        points.removeAt(i);
    }
}

void Capture_region::create_point(int index)
{
    if(points.size() > index)
    {
        return;
    }
    else if(free_points.size() >= 1)
    {
        points.append(free_points.takeLast());
        free_points.removeLast();
    }
    else
    {
        Stretch_point* temp_point = new Stretch_point(region, parent);
        this->points.append(temp_point);
    }
}

void Capture_region::on_pos_change(int pos)
{
    for(int i=0; i<points.size(); i++)
    {
        points[i]->set_pos(pos);
    }
}

