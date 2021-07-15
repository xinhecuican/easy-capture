#include "stretch_point.h"
#include<QPainter>
#include "Manager/config.h"
#include<QDebug>
#include <iostream>
#include "Helper/debug.h"


Stretch_point::Stretch_point()
{
    set_style();
}

Stretch_point::~Stretch_point()
{

}

Stretch_point::Stretch_point(Ipoint_position_change* region, QPoint point)
{
    //this->point = point;
    this->region = region;
    set_style();
    move(point.x()-OFFSET, point.y()-OFFSET);
}

Stretch_point::Stretch_point(Ipoint_position_change* region, QWidget* parent, QPoint point) : QPushButton(parent)
{
    //this->point = point;
    this->parent = parent;
    this->region = region;
    set_style();
    move(point.x()-OFFSET, point.y()-OFFSET);
}

Stretch_point::Stretch_point(Ipoint_position_change* region, QWidget* parent):QPushButton(parent)
{
    this->parent = parent;
    this->region = region;
    set_style();
}

void Stretch_point::set_style(QString name)
{

    index_count = 0;
    is_constraints_effect = false;
    QFile file(":/qss/button/capture_window_button/" + name);
    file.open(QFile::ReadOnly);
    QString str = file.readAll();
    this->setStyleSheet(str);
    file.close();
}

//void Stretch_point::paintEvent(QPaintEvent *event)
//{
    /*QPainter painter(this);
    painter.setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform, true);
    QPainterPath path;
    QBrush brush(QColor(247, 245, 244));
    path.addEllipse(this->pos(), 30, 30);
    painter.fillPath(path, brush);
    painter.setBrush(Qt::white);
    painter.drawEllipse(this->pos(), 30, 30);*/

//}

void Stretch_point::mousePressEvent(QMouseEvent* event)
{
    event->accept();
    begin_point = event->globalPos();
    region->position_change_begin(m_pos, begin_point);
}

void Stretch_point::mouseMoveEvent(QMouseEvent *event)
{
    event->accept();
    /*
     *
     * TODO: 区域随着点的移动而移动
     */
    int dx = event->globalX() - begin_point.x();
    int dy = event->globalY() - begin_point.y();
    //QPoint angle_point = get_angle_point();
    if(is_constraints_effect)
    {
        if(dx + this->pos().x() < constraints[0] - OFFSET)
        {
            dx = constraints[0] - this->pos().x() - OFFSET;
        }
        if(dx + this->pos().x() > constraints[2] + OFFSET)
        {
            dx = constraints[2] - this->pos().x() + OFFSET;
        }
        if(dy + this->pos().y() < constraints[1] - OFFSET)
        {
            dy = constraints[1] - this->pos().y() - OFFSET;
        }
        if(dy + this->pos().y() > constraints[3] + OFFSET)
        {
            dy = constraints[3] - this->pos().y() + OFFSET;
        }
    }

    translate(dx, dy);
    begin_point = event->globalPos();
    if(dx != 0)
    {
        h_neigh->on_neigh_move(true, dx);
    }
    if(dy != 0)
    {
        v_neigh->on_neigh_move(false, dy);
    }
    region->control_point_position_change(position, get_index(), dx, dy);
    parent->update();//不能删去，否则不会绘制边界
}

void Stretch_point::mouseReleaseEvent(QMouseEvent* event)
{
    region->position_change_end(event->globalPos());
}

void Stretch_point::on_neigh_move(bool is_hneigh, int sum)
{
    if(is_hneigh)
    {
        move(pos().x() + sum, pos().y());
        region->control_point_position_change(position, get_index(), sum , 0);
    }
    else
    {
        move(pos().x(), pos().y() + sum);
        region->control_point_position_change(position, get_index(), 0, sum);
    }
}

void Stretch_point::change_to_position(QPoint &point)
{
    //this->point = point;
    move(point.x()-OFFSET, point.y()-OFFSET);
}

void Stretch_point::set_neigh(Stretch_point* point)
{
    if(this->pos().x() == point->getx())
    {
        h_neigh = point;
    }
    else if(this->pos().y() == point->gety())
    {
        v_neigh = point;
    }
}

void Stretch_point::set_node(QPoint point)
{
    move(point.x()-OFFSET, point.y()-OFFSET);
    index_count = 0;
}

void Stretch_point::set_index(int index)
{
    if(index_count >= 10)
    {
        Debug::show_error_message("重复节点数量达到上限\n位置: Stretch_point::set_index");
        return;
    }
    this->index[index_count++] = index;
}

void Stretch_point::set_pos(int position)
{
    this->position = position;
}

void Stretch_point::translate(int dx, int dy)
{
    move(this->pos().x() + dx, this->pos().y() + dy);
}

int Stretch_point::getx()
{
    return pos().x();
}

int Stretch_point::gety()
{
    return pos().y();
}

QList<int> Stretch_point::get_index()
{
    QList<int> ans = QList<int>();
    for(int i=0; i<index_count; i++)
    {
        ans.append(index[i]);
    }
    return ans;
}

QPoint Stretch_point::get_node()
{
    return pos();
}

QPoint Stretch_point::get_angle_point()
{
    return QPoint(v_neigh->getx(), h_neigh->gety());
}

void Stretch_point::set_constraints(int minx, int miny, int maxx, int maxy)
{
    is_constraints_effect = true;
    constraints[0] = minx;
    constraints[1] = miny;
    constraints[2] = maxx;
    constraints[3] = maxy;
}

void Stretch_point::region_move(int dx, int dy)
{
    translate(dx, dy);
    if(dx != 0)
    {
        h_neigh->on_neigh_move(true, dx);
    }
    if(dy != 0)
    {
        v_neigh->on_neigh_move(false, dy);
    }
    region->control_point_position_change(position, get_index(), dx, dy);
    parent->update();//不能删去，否则不会绘制边界
}
