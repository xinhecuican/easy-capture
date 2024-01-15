#ifndef CLIPLAYERBASE_H
#define CLIPLAYERBASE_H
#include "layerbase.h"

class ClipLayerBase : public LayerBase
{
    Q_OBJECT
public:
    ClipLayerBase(const QString& name,
                  ILayerControl* manager,
                  QGraphicsItem* parent) :
          LayerBase(name, manager, parent) {}
    virtual ~ClipLayerBase(){}
    virtual QRectF getClipRect()=0;
    virtual QPainterPath getClipPath()=0;
signals:
    void boundChangeBegin();
    void boundChange(QRectF rect);
};

#endif // CLIPLAYERBASE_H
