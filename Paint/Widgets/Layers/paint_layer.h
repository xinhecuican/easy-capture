#ifndef PAINT_LAYER_H
#define PAINT_LAYER_H
#include<QString>
#include<QPicture>
#include<QHash>
#include<QPainterPath>
#include "Base/Record_element.h"
#include "Paint/Data/paint_data.h"
#include "Paint/Data/Ilayer.h"
#include<QWidget>
#include "Paint/Widgets/Recorder_element/paint_record.h"
#include "Paint/Data/Common.h"
#include<QGraphicsItem>

class Paint_layer : public QGraphicsObject
{
    Q_OBJECT
public:
    Paint_layer(QGraphicsItem* parent=nullptr);
    ~Paint_layer();
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = nullptr) override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;
    void setEnableDraw(bool enable);
    void reset();
    void setErase(bool enable);
    Q_INVOKABLE void undoRedoPaintFunc(bool is_undo, PaintItem* item);
    QRectF boundingRect() const override;
private:
    void removeLines(QPointF point);
    QList<PaintItem*> lines;
    PaintItem* current_item;
    QPainterPath path;
    QGraphicsItem* parent;
    bool is_enable;
    bool is_press;
    bool is_erase;
};

#endif // PAINT_LAYER_H
