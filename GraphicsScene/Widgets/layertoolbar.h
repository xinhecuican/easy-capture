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
    void addContainer(QToolButton* button, std::function<void()>const& f, std::function<void()>const& leavef, bool exclude=false);
    void moveTo(const QRectF& mask);
    void setBound(const QRectF& bound);
    void setContainer(LayerContainer* container);
    void hideAll();
    void showAll();
    void simulateClick(int id);
    LayerContainer* getCurrentGroupContainer();
    virtual void reset();

public slots:
    void onGroupClick(int id);

private:
    void execValid(int id);
    void execInvalid(int id);

protected:
    PaintArea* area;
    QButtonGroup* group;
    int groupId;
    AttributeToolbar* attributeBar;
    QList<LayerContainer*> containers;
    QMap<int, std::function<void()>> validF;
    QMap<int, std::function<void()>> invalidF;
    QRectF mask;
    QRectF bound;
    int beforeId;
};

#endif // LAYERTOOLBAR_H
