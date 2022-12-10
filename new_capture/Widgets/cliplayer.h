#ifndef CLIPLAYER_H
#define CLIPLAYER_H
#include <QGraphicsObject>
#include "Paint/Widgets/Layers/baselayer.h"
#include "clipregion.h"
#include "masklayer.h"
#include <QPainterPath>
#include "Paint/Widgets/Layers/picture_layer.h"
#include <QToolBar>
#include <QToolButton>
#include <QComboBox>
#include "Style_widget/colorwidget.h"
#include "attributetoolbar.h"
#include "Paint/Data/paint_data.h"
#include "mosicsample.h"
#include <QSpinBox>
#include <QSlider>
#include "Helper/EnumReflect.h"

class ClipLayer : public QGraphicsObject
{
    Q_OBJECT
public:
    MAKE_ENUM(MiddleButtonType,
            PaintWindow,
            Save2Clip,
            Save2File
    )
    ClipLayer(QWidget* widget_parent, QGraphicsScene* scene, QGraphicsItem* parent=nullptr);
    ~ClipLayer();
    QRectF boundingRect()const override;
    QPainterPath shape()const override;
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent* event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = nullptr) override;
    void setEnable(bool enable);
    void prepareSave();
    void endSave();
    void capture(QPixmap pix);
    void setPic(QPixmap pix);
    QRectF getClipRect();
    void reset();
    void sendRequestImage();
    void setWidgetParent(QWidget* parent);
    void buttonEnter(int id);
signals:
    void requestImage();
    void needSave();
    void needClip();
    void paintShape(SHAPE_TYPE type);
    void stateChange(PAINT_STATE state);
    void mosaicChange(bool is_range, int value);
    void needReset();
private:
    void setToolBar();
    void setAttributeToolbar();
    void calBarPos();
    void updateAttributeToolbar(int id);
    void onToolbarButtonClick(int id);
    QPolygonF polygon;
    QPointF begin_point;
    QPointF end_point;
    QRect screen_rect;
    QWidget* widget_parent;
    bool is_drag;
    bool begin_clip;
    bool is_save;
    bool is_enable;
    int before_id;
    Qt::MouseButton button;
    int drag_index;
    QList<ClipRegion*> regions;
    QPixmap pix;
    QPixmap savePix;
    MaskLayer* mask_layer;
    QToolBar* toolbar;
    QGraphicsScene* scene;
    AttributeToolbar* attribute_toolbar;
    QToolButton* save_button;
    QToolButton* clip_button;
    QToolButton* erase_button;
    QToolButton* ok_button;
    QToolButton* cancel_button;
    QToolButton* undo_button;
    QToolButton* redo_button;
    QToolButton* cursor_button;
    QToolButton* pencil_button;
    QToolButton* highlighter_button;
    QToolButton* rect_button;
    QToolButton* text_button;
    QToolButton* shape_button;
    QToolButton* mosaic_button;
    QButtonGroup* button_group;

    QComboBox* width_button;
    Paint_data rect_setting;
    Paint_data pencil_setting;
    Paint_data highlighter_setting;
    ColorWidget* color_widget;
    MosicSample* mosaic_sample;
    QSpinBox* mosaic_size;
    QSlider* mosaic_range;
    QToolButton* rect_capture;
    QToolButton* free_capture;
    QToolButton* arrow_button;
};

#endif // CLIPREGION_H
