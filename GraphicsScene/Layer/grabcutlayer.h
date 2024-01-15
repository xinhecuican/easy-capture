#ifndef GRABCUTLAYER_H
#define GRABCUTLAYER_H
#include "layerbase.h"
#include "../Widgets/grabcuthandler.h"

class GrabcutLayer : public LayerBase
{
public:
    GrabcutLayer(const QString& name,
                 ILayerControl* manager,
                 QGraphicsItem* parent=nullptr);
    void reset() override;
    void addPoint(const QPointF& point, bool background=true, bool isEnd=false);
    void makeEnable();
    void setEnable(bool enable, int index=0) override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = nullptr) override;
    QRectF boundingRect()const override;
    int type() const override;
    int getZValue() const override;
    void setEnableShow();
    QRectF getSaveRect() override;
    void prepareSave() override;
    void onDelete(const QPointF &point) override;
    void startErase() override;
private:
    void deletePath(const QPointF& point);
    QList<QPainterPath> foregroundPaths;
    QList<QPainterPath> backgroundPaths;
    QPainterPath currentPath;
    QRectF bound;
    bool background;
    GrabcutHandler* handler;
    bool enableShow;
};

#endif // GRABCUTLAYER_H
