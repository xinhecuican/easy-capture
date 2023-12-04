#ifndef LAYERMANAGER_H
#define LAYERMANAGER_H
#include <QGraphicsScene>
#include <QWidget>
#include "Layer/layerbase.h"

class LayerManager
{
public:
    LayerManager();
    virtual ~LayerManager(){}
    virtual void addLayer(LayerBase* layer);
    virtual LayerBase* removeLayer(const QString& name);
    virtual void changeFocusLayer(const QString& name);
    virtual void changeFocusLayer(LayerBase* layer);
    bool hasFocus(LayerBase* layer);
    LayerBase* getFocusLayer();
    QList<LayerBase*> getLayers() const;
protected:
    QHash<QString, LayerBase*> layers;
    LayerBase* focusLayer;
};

#endif // LAYERMANAGER_H
