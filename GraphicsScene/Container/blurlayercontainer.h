#ifndef BLURLAYERCONTAINER_H
#define BLURLAYERCONTAINER_H
#include "layercontainer.h"
#include "../Layer/blurlayer.h"
#include "../Layer/grabcutlayer.h"
#include "../Layer/maskcolorlayer.h"
#include <QComboBox>
#include "../Widgets/colorpicker.h"

class BlurLayerContainer : public LayerContainer
{
public:
    BlurLayerContainer(PaintArea* area);
    void layerMousePressEvent(QGraphicsSceneMouseEvent *event) override;
    void layerMouseMoveEvent(QGraphicsSceneMouseEvent *event) override;
    void layerMouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;
    QWidget * onValid(QWidget *widgetParent) override;
    void onInvalid() override;
    int getCurrentType();
    void reset() override;

private:
    void addColor(QColor color);

private:
    BlurLayer* blurLayer;
    GrabcutLayer* grabcutLayer;
    MaskColorLayer* maskColorLayer;
    int type;
    int windowSize;
    bool background;
    QComboBox* maskColorCombo;
    ColorPicker* colorPicker;
    bool valid;
    QToolButton* helpButton;
    QToolButton* removeButton;
};

#endif // BLURLAYERCONTAINER_H
