#ifndef CAPTURE_AREA_H
#define CAPTURE_AREA_H
#include<QWidget>
#include<QDebug>
#include<QMouseEvent>
#include<QPoint>
#include<QRegion>
#include<Helper/my_math.h>
#include<QList>
#include"stretch_point.h"
#include "Helper/plist.h"

class Capture_area : QWidget
{
public:
    struct Capture_region
    {
        QWidget* parent;
        Capture_area* p_region;
        QPolygon polygon;
        PList<Stretch_point*> points;
        PList<Stretch_point*> free_points;

        Capture_region()
        {
            polygon = QPolygon();
            points = PList<Stretch_point*>();
        }

        Capture_region(Capture_area* p_region, QWidget* parent)
        {
            this->parent = parent;
            this->p_region = p_region;
            polygon = QPolygon();
            points = PList<Stretch_point*>();
            free_points = PList<Stretch_point*>();
        }

        Capture_region(Capture_area* p_region, QWidget* parent, Capture_region region)
        {
            this->p_region = p_region;
            this->parent = parent;
            polygon = QPolygon(region.polygon);
            points = PList<Stretch_point*>();
            free_points = PList<Stretch_point*>();
            set_nodes();
        }

        Capture_region(Capture_area* p_region, QWidget* parent, QRect rect)
        {
            this->p_region = p_region;
            this->parent = parent;
            polygon = QPolygon();
            points = PList<Stretch_point*>();
            free_points = PList<Stretch_point*>();
            unit(rect);
        }

        void clear()
        {
            polygon.clear();
            points.clear_all();
        }

        ~Capture_region()
        {
        }

        void unit(Capture_region &region)
        {
            polygon = polygon.united(region.polygon);
            set_nodes();
        }

        void unit(QRect &rect)
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

        bool intersected(Capture_region &region)
        {
            return !this->polygon.intersected(region.polygon).isEmpty();
        }

        bool intersected(QRect &rect)
        {
            return !this->polygon.intersected(rect).isEmpty();
        }

        void move(int dx, int dy)
        {
            polygon.translate(dx, dy);
            for(int i=0; i<points.size(); i++)
            {
                points[i]->translate(dx, dy);
            }
        }

        void point_move(QList<int> position, int dx, int dy)
        {
            for(int i=0; i<position.size(); i++)
            {
                polygon[position[i]].setX(polygon[position[i]].x() + dx);
                polygon[position[i]].setY(polygon[position[i]].y() + dy);
            }
        }

        bool contains(QPoint point)
        {
            return polygon.containsPoint(point, Qt::OddEvenFill);
        }

        QPolygon get_polygon()
        {
            return polygon;
        }

        void set_nodes()
        {
            QList<QPoint> list = polygon.toList();
            if(list.size() == 0)
            {
                return;
            }
            int len1 = list.size();
            int points_len = len1;


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
                    points_len--;
                    if(i+1 < len1)
                    {
                        now = points[i+1];
                        now->set_node(QPoint(-1, -1));
                    }
                    if(i+2 >= len1)
                    {
                        if(now != points[0])
                            points[0]->set_index(i+1);
                        break;
                    }
                }
                else
                {
                    create_point(i);
                    points[i]->show();
                    points[i]->set_node(list[i]);
                    points[i]->set_neigh(points[i-1]);
                    points[i]->set_index(i);
                    points[i-1]->set_neigh(points[i]);
                }
            }
            //多余节点转换为空闲节点
            for(int i=points_len; i<points.size(); i++)
            {
                free_points.append(points[i]);
                points[i]->hide();
            }
            points.remove_range(points_len, points.size()-1);

        }

        void create_point(int index)
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
                Stretch_point* temp_point = new Stretch_point(p_region, parent);
                this->points.append(temp_point);
            }
        }

        void on_pos_change(int pos)
        {
            for(int i=0; i<points.size(); i++)
            {
                points[i]->set_pos(pos);
            }
        }
    };
    Capture_area(QWidget* parent);
    ~Capture_area();
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void paintEvent(QPaintEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;

    void reset();
    bool region_contain(QPoint p);
    void add_region(Capture_region &region);
    void on_region_remove(Capture_region &region);
    void control_point_position_change(int index, QList<int> position, int dx, int dy);

    int get_x();
    int get_y();
    int get_w();
    int get_h();
    QList<Capture_area::Capture_region> get_region();
    int get_region_count();
    bool is_begin_draw();
    bool is_press_region();

    QPoint begin_point;
    QPoint end_point;
private:
    //QList<QRegion> regions;
    bool begin_draw;
    QRect all_rect;
    void combine_region(QRect rect);
    void combine_region(Capture_region region);
    bool key_press;
    bool _is_press_region;
    QWidget* parent;
    int now_index = 0;

    QList<Capture_region> regions;
};

#endif // CAPTURE_AREA_H
