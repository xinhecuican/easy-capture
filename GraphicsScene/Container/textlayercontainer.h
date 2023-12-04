#ifndef TEXTLAYERCONTAINER_H
#define TEXTLAYERCONTAINER_H
#include "layercontainer.h"
#include "../Widgets/FontData.h"
#include <QColorDialog>
#include <QComboBox>
#include <QStandardItemModel>
#include <QCompleter>


class TextLayerContainer : public LayerContainer
{
public:
    TextLayerContainer(PaintArea* area);
    void layerMousePressEvent(QGraphicsSceneMouseEvent *event) override;
    QWidget * onValid(QWidget *widgetParent) override;

private:
    QList<QString> get_font_text();
    int textId;
    FontData data;
    QToolButton* colorSelectButton;
    QComboBox* fontBox;
    QComboBox* widthBox;
    QToolButton* boldButton;
    QToolButton* italicButton;
    QToolButton* underlineButton;
};

#endif // TEXTLAYERCONTAINER_H
