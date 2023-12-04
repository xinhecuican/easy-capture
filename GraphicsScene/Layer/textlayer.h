#ifndef TEXTLAYER_H
#define TEXTLAYER_H
#include "layerbase.h"
#include "../Widgets/textitem.h"

class TextLayer : public LayerBase
{
    Q_OBJECT
public:
    TextLayer(const QPointF& pos,
              const QString& name,
              ILayerControl* manager,
              QGraphicsItem* parent=nullptr);
    void reset() override;
    void getFocus() override;
    void loseFocus() override;
    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;
    void prepareSave() override;
    void setParameter(const QString &name, const QVariant &var) override;
    void setEnable(bool enable) override;
    const QString getHelp() override;
    bool contains(const QPointF& point)const override;

signals:
    void formatChange(const QFont& font, const QBrush& brush);

private:
    TextItem* textItem;
};

#endif // TEXTLAYER_H
