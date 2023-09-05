#ifndef PAINT_AREA_H
#define PAINT_AREA_H
#include<QWidget>
#include "Paint/Widgets/Layers/picture_layer.h"
#include<QBoxLayout>
#include<QVector>
#include "Paint/Widgets/Layers/paintlayer.h"
#include "Paint/Data/Ilayer.h"
#include "Paint/Data/Common.h"
#include<QScrollArea>
#include<QMainWindow>
#include<QGraphicsScene>
#include "Paint/Widgets/Layers/shapelayer.h"
#include "Paint/Data/History_data.h"
#include "new_capture/Widgets/cliplayer.h"
#include <QProcess>

/**
 * @brief 绘图管理类，用来控制图片的显示，保存，状态管理等
 */
class PaintArea : public QGraphicsScene {
    Q_OBJECT
public:
    PaintArea(QWidget* parent=nullptr, bool enable_clip=false);
    void setPic(QPixmap pic, QRect rect);
    void setClipPic(QPixmap pix);
    void deleteShape();
    void paintShape(SHAPE_TYPE type);
    void reset();
    void stateChange(PAINT_STATE state);
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;
    bool save(History_data::save_type type, QString path);
    bool save2Clipboard();
    bool needSave();
    void prepareSave();
    void endSave();
    void sendRequestImage();
    void onViewSet(QWidget* view);
    void clipButtonEnter(int id);
    void startOcr();
//    void wheelEvent(QGraphicsSceneWheelEvent *event) override;
    RecordInfo getRecordInfo();
private:
    void setOtherLayer();
    void initSettingPanel();
    void initProcess();

    const int DEFAULT_LAYER_NUM = 1;
    SHAPE_TYPE shape_type;
    PAINT_STATE state;
    QPointF begin_point;
     PaintLayer* paint_layer;
    Picture_layer* pic_layer;
    ShapeLayer* shape_layer;
    ClipLayer* clip_layer;
    bool is_save;
    bool is_clip;
    QProcess ocrProcess;
    QProcess showOcrResultProcess;
};

#endif // PAINT_AREA_H
