#ifndef PAINTLAYER_H
#define PAINTLAYER_H
#include "layerbase.h"
#include <QList>
#include <QPainterPath>
#include <QPen>

class PaintLayer : public LayerBase {
public:
    enum PaintMethod{PaintPath, Smooth};
    PaintLayer(const QString& name,
               ILayerControl* manager,
               QGraphicsItem* parent=nullptr);
    void reset() override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = nullptr) override;
    QRectF boundingRect()const override;
    QPainterPath shape()const override;
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
    void addPoint(const QPointF& point);
    void setMethod(PaintMethod method);
    void setParameter(const QString &name, const QVariant &var) override;
    void setEnable(bool enable) override;
private:
    QList<QPointF> points;
    PaintMethod method;
    QPainterPath path;
    QPen pen;
};

class SmoothCurveGenerator {
public:
    /**
* @brief generateSmoothCurve 的重载函数
*/
    static QPainterPath generateSmoothCurve(QList<QPointF> points, bool closed = false, double tension = 0.5, int numberOfSegments = 16);

/**
* @brief 使用传入的曲线顶点坐标创建平滑曲线。
*
* @param points 曲线顶点坐标数组，
* points[i+0] 是第 i 个点的 x 坐标，
* points[i+1] 是第 i 个点的 y 坐标
* @param closed 曲线是否封闭，默认不封闭
* @param tension 密集程度，默认为 0.5
* @param numberOfSegments 平滑曲线 2 个顶点间的线段数，默认为 16
* @return 平滑曲线的 QPainterPath
*/
    static QPainterPath generateSmoothCurve(QList<double>points, bool closed = false, double tension = 0.5, int numberOfSegments = 16);
};

#endif // PAINTLAYER_H
