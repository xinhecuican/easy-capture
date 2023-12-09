#ifndef LAYERBASE_H
#define LAYERBASE_H
#include <QGraphicsObject>
#include "../ilayercontrol.h"
#include <QPainter>
#include <QGraphicsSceneMouseEvent>

class LayerBase : public QGraphicsObject
{
    Q_OBJECT
public:
    explicit LayerBase(const QString& name, ILayerControl* manager, QGraphicsItem* parent=nullptr);
    virtual ~LayerBase(){}
    bool isEnable();
    bool isErase();
    QString getName()const;
    virtual const QString getHelp(){return "";};
    virtual void setErase(bool enable);
    virtual void setEnable(bool enable, int index=0);
    virtual void setParameter(const QString& name, const QVariant& var){};
    virtual void prepareSave();
    virtual void endSave();
    virtual void reset();
    virtual int getZValue() const;
    virtual void getFocus(){}
    virtual void loseFocus(){}
    virtual void onImageSet(){}

protected:
    QString name;
    ILayerControl* manager;
    bool erase;
    bool enable;
    bool isSaving;
};

#endif // LAYERBASE_H
