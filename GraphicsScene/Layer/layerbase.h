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
    QString getName()const;
    virtual const QString getHelp(){return "";};
    virtual void setEnable(bool enable, int index=0);
    virtual void setParameter(const QString& name, const QVariant& var){};
    virtual void prepareSave();
    virtual void endSave();
    virtual void reset();
    virtual int getZValue() const;
    virtual void getFocus(){}
    virtual void loseFocus(){}
    virtual void onImageSet(){}
    virtual QRectF getSaveRect(){return boundingRect();}
    virtual void onDelete(const QPointF& point){}
    virtual void startErase(){}
    virtual void release(){manager->removeThis(this);}

protected:
    QString name;
    ILayerControl* manager;
    bool enable;
    bool isSaving;
};

#endif // LAYERBASE_H
