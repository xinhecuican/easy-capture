#ifndef PAINT_AREA_H
#define PAINT_AREA_H
#include<QWidget>
#include "Paint/Widgets/Layers/picture_layer.h"
#include<QBoxLayout>
#include<QVector>
#include "Paint/Widgets/Layers/paint_layer.h"
#include "Paint/Data/Ilayer.h"
#include "Paint/Data/Common.h"
#include<QScrollArea>
#include<QMainWindow>
#include<QGraphicsScene>
#include "Paint/Widgets/Layers/shapelayer.h"
#include "Paint/Data/History_data.h"

class Paint_area : public QGraphicsScene
{
    Q_OBJECT
public:
    Paint_area(QObject* parent=nullptr);
    void setPic(QPixmap pic, QRect rect);
    void deleteShape();
    void paintShape(SHAPE_TYPE type);
    void reset();
    void stateChange(PAINT_STATE state);
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;
    void save(History_data::save_type type, QString path);
    void save2Clipboard();
    bool needSave();
private:
    void setOtherLayer();
    void initSettingPanel();
    void prepareSave();
    void endSave();

    const int DEFAULT_LAYER_NUM = 1;
    SHAPE_TYPE shape_type;
    PAINT_STATE state;
    QPointF begin_point;
    Paint_layer* paint_layer;
    Picture_layer* pic_layer;
    ShapeLayer* shape_layer;
    bool is_save;
};

#endif // PAINT_AREA_H
