#include "clipregion.h"
#include <QGraphicsSceneMouseEvent>
#include <QDebug>

ClipRegion::ClipRegion(QPolygonF polygon, QGraphicsItem* parent) : QGraphicsObject(parent)
{
    this->polygon = polygon;
    isButtonHide = false;
    setFlag(QGraphicsItem::ItemIsMovable);
    remapPoint();
}

ClipRegion::ClipRegion(QPolygonF polygon, bool isButtonHide, QGraphicsItem* parent) : QGraphicsObject(parent)
  , isButtonHide(isButtonHide)
  , polygon(polygon)
{
    setFlag(QGraphicsItem::ItemIsMovable);
    emit regionChange();
}

ClipRegion::~ClipRegion()
{
}

QRectF ClipRegion::boundingRect() const
{
    return polygon.united(childrenBoundingRect()).boundingRect();
}

QPainterPath ClipRegion::shape() const
{
    QPainterPath path;
    path.addPolygon(polygon);
    return path;
}

void ClipRegion::addRect(QRectF rect)
{
    this->polygon = polygon.united(rect);
    remapPoint();
}

void ClipRegion::addPolygon(QPolygonF polygon)
{
    this->polygon = this->polygon.united(polygon);
    remapPoint();
}

void ClipRegion::united(QPolygonF polygon)
{
    this->polygon = this->polygon.united(polygon);
    remapPoint();
}

void ClipRegion::united(ClipRegion* region)
{
    this->polygon = polygon.united(region->polygon);
    remapPoint();
}

bool ClipRegion::intersect(QRectF rect)
{
    return polygon.intersects(rect);
}

bool ClipRegion::intersect(QPolygonF polygon)
{
    return this->polygon.intersects(polygon);
}

void ClipRegion::remapPoint()
{
    if(isButtonHide)
        return;
    for(int i=buttons.size(); i<polygon.count(); i++)
    {
        ExpandButton* button = new ExpandButton(this);
        connect(button, static_cast<void (ExpandButton::*)(QList<int>, qreal, qreal, int, int)>(&ExpandButton::posChange), this, [=](QList<int> index, qreal dx, qreal dy, int x_neigh, int y_neigh){
            for(int i=0; i<index.size(); i++)
            {
                polygon[index[i]] += QPointF(dx, dy);
            }
            QList<int> neigh_index = buttons[x_neigh]->getIndex();
            for(int i=0; i<neigh_index.size(); i++)
            {
                polygon[neigh_index[i]] += QPointF(dx, 0);
            }
            neigh_index.clear();
            neigh_index = buttons[y_neigh]->getIndex();
            for(int i=0; i<neigh_index.size(); i++)
            {
                polygon[neigh_index[i]] += QPointF(0, dy);
            }
            buttons[x_neigh]->setPosition(buttons[x_neigh]->pos() + QPointF(dx, 0));
            buttons[y_neigh]->setPosition(buttons[y_neigh]->pos() + QPointF(0, dy));
            update();
        });
        buttons.append(button);
    }
    for(int i=0; i<buttons.count(); i++)
    {
        buttons[i]->hide();
        buttons[i]->clearIndex();
    }
    QList<QPoint> pos_list = polygon.toPolygon().toList();
    int button_num = 0;
    for(int i=0; i<pos_list.size(); i++)
    {
        bool success = false;
        for(int k=0; k<button_num; k++)
        {
            if(buttons[k]->getPos() == pos_list[i])
            {
                buttons[k]->setIndex(i);
                success = true;
                if(buttons[k]->setNeighbor(buttons[button_num-1]->getPos(), button_num-1))
                {
                    buttons[button_num-1]->setNeighbor(buttons[k]->getPos(), k);
                }
                break;
            }
        }
        if(!success)
        {
            buttons[button_num]->show();
            buttons[button_num]->setIndex(i);
            buttons[button_num]->setIntPos(pos_list[i]);
            buttons[button_num]->setPosition(polygon[i]);
            for(int k=button_num-1; k>=0; k--)
            {
                if(buttons[button_num]->setNeighbor(buttons[k]->getPos(), k))
                {
                    buttons[k]->setNeighbor(buttons[button_num]->getPos(), button_num);
                    break;
                }
            }
            button_num++;
        }
    }
    emit regionChange();
}

QPolygonF ClipRegion::getPolygon()
{
    return polygon;
}

void ClipRegion::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{

}

bool ClipRegion::contain(QPointF point)
{
    return polygon.contains(point);
}

void ClipRegion::move(qreal dx, qreal dy)
{
    polygon.translate(dx, dy);
    for(int i=0; i<buttons.size(); i++)
    {
        buttons[i]->moveBy(dx, dy);
    }
    emit regionChange();
    update();
}

void ClipRegion::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    begin_point = event->pos();
}

void ClipRegion::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    QPointF delta = event->pos() - begin_point;
    begin_point = event->pos();
    move(delta.x(), delta.y());
}

void ClipRegion::hideNormal()
{
    for(ExpandButton* button: buttons)
    {
        button->hide();
    }
    update();
}

void ClipRegion::showNormal()
{
    for(ExpandButton* button: buttons)
    {
        button->show();
    }
    update();
}

void ClipRegion::hideButton()
{
    isButtonHide = true;
    for(ExpandButton* button: buttons)
    {
        button->hide();
    }
    update();
}
