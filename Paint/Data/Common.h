#ifndef COMMON_H
#define COMMON_H
#include "paint_data.h"
#include<QPainterPath>
#include<QFont>
#include<cmath>
enum SHAPE_TYPE{TEXT, DELETE_SHAPE, BLUR, RECTANGLE, PAINT_ARROW, MASK};
enum PAINT_STATE{PAINT, ARROW, SHAPE, ERASE};
enum direction{NW, NE, SW, SE, N, W, S, E};

struct font_data
{
    QFont font;
    QColor color;
};

struct paint_node
{
    int len;
    font_data data;
    paint_node* next;
};

struct paint_info
{
    paint_info(){};
    paint_info(Paint_data* style, QPainterPath paint_path)
    {
        style_info = style;
        path = paint_path;
    }
    Paint_data* style_info;
    QPainterPath path;
};

struct Line
{
    QPoint bp;
    QPoint ep;
    Line(){};
    Line(QPoint begin, QPoint end)
    {
        bp = begin;
        ep = end;
    }

    float cross_product(Line line)
    {
        int x1 = ep.x() - bp.x();
        int y1 = ep.y() - bp.y();
        int x2 = line.ep.x() - line.bp.x();
        int y2 = line.ep.y() - line.bp.y();
        return x1 * y2 - x2 * y1;
    }

    float cross_product(Line l1, Line l2)
    {
        int x1 = l1.ep.x() - l1.bp.x();
        int y1 = l1.ep.y() - l1.bp.y();
        int x2 = l2.ep.x() - l2.bp.x();
        int y2 = l2.ep.y() - l2.bp.y();
        return x1 * y2 - x2 * y1;
    }

    Line normalize(Line line)
    {
        QPoint v = line.ep - line.bp;
        float len = v.x() * v.x() + v.y() * v.y();
        v.setX((float)v.x() / len);
        v.setY((float)v.y() / len);
        Line ans;
        ans.bp = QPoint(0, 0);
        ans.ep = v;
        return ans;
    }

    Line normalize()
    {
        QPoint v = ep - bp;
        float len = v.x() * v.x() + v.y() * v.y();
        v.setX((float)v.x() / len);
        v.setY((float)v.y() / len);
        Line ans;
        ans.bp = QPoint(0, 0);
        ans.ep = v;
        return ans;
    }

    float angle(Line line)
    {
        Line normal = normalize(line);
        Line this_normal = normalize();
        return asin(cross_product(this_normal, normal));
    }
};
#endif // COMMON_H
