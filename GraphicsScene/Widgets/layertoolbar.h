#ifndef LAYERTOOLBAR_H
#define LAYERTOOLBAR_H
#include <QToolBar>
#include "../Container/layercontainer.h"
#include "attributetoolbar.h"
#include "../paintarea.h"

class LayerToolBar : public QToolBar
{
public:
    LayerToolBar(PaintArea* area, QWidget* parent=nullptr);
    void addContainer(QToolButton* button, LayerContainer* container, bool exclude=false);
    void addContainer(const QString& icon,
                      const QString& tip,
                      LayerContainer* container,
                      bool exclude = false);
    void moveTo(const QRectF& mask);
    void setBound(const QRectF& bound);
    void setContainer(LayerContainer* container);
    void hideAll();
    void showAll();

private slots:
    void onGroupClick(int id);

private:
    PaintArea* area;
    QButtonGroup* group;
    int groupId;
    AttributeToolbar* attributeBar;
    QList<LayerContainer*> containers;
    QRectF mask;
    QRectF bound;
};

#endif // LAYERTOOLBAR_H
