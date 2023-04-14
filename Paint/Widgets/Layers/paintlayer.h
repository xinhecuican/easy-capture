#ifndef PAINT_LAYER_H
#define PAINT_LAYER_H
#include<QString>
#include<QPicture>
#include<QHash>
#include<QPainterPath>
#include "Base/RecordElement.h"
#include "Paint/Data/paint_data.h"
#include "Paint/Data/Ilayer.h"
#include<QWidget>
#include "Paint/Widgets/Recorder_element/paint_record.h"
#include "Paint/Data/Common.h"
#include<QGraphicsItem>
#include <QPainter>

class  PaintLayer : public QGraphicsObject {
    Q_OBJECT
public:
    PaintLayer(QGraphicsItem* parent=nullptr);
    ~PaintLayer();
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = nullptr) override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;
    void setEnableDraw(bool enable);
    void setPix(const QPixmap& pix, QPoint pos);
    void reset();
    void setErase(bool enable);
    Q_INVOKABLE void undoRedoPaintFunc(bool is_undo, PaintItem* item);
    QRectF boundingRect() const override;
    int type() const override;
private:
    void removeLines(QPointF point);
    void updateLines();
    QList<PaintItem*> lines;
    PaintItem* current_item;
    QPainterPath path;
    QGraphicsItem* parent;
    bool is_enable;
    bool is_press;
    bool is_erase;
    int updateAnchor;
    QPixmap pix;
    QPainter* pixPainter;
    bool _isPixSet;
};

#endif // PAINT_LAYER_H
