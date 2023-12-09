#include "defaulttoolbar.h"
#include "../Container/blurlayercontainer.h"
#include "../Container/geolayercontainer.h"
#include "../Container/masklayercontainer.h"
#include "../Container/paintlayercontainer.h"
#include "../Container/textlayercontainer.h"

DefaultToolbar::DefaultToolbar(PaintArea* area, QWidget* parent) : LayerToolBar(area, parent)
{
    GeoLayerContainer* geoLayerContainer = new GeoLayerContainer(area);
    addContainer("shape", "", geoLayerContainer, true);
    BlurLayerContainer* blurLayerContainer = new BlurLayerContainer(area);
    addContainer("mosaic", "", blurLayerContainer, true);
    QToolButton* cursorButton = new QToolButton(this);
    cursorButton->setIcon(QIcon(":/image/cursor.png"));
    cursorButton->setToolTip(MString::search("{l4yTU9QXUd}指针"));
    cursorButton->setCursor(QCursor(QPixmap(":/image/cursor.png"), 0, 0));
    MaskLayerContainer* maskLayerContainer = new MaskLayerContainer(area);
    addContainer(cursorButton, maskLayerContainer, true);
    setContainer(maskLayerContainer);
    PaintLayerContainer* paintLayerContainer = new PaintLayerContainer(area);
    addContainer("pencil", "", paintLayerContainer, true);
    TextLayerContainer* textLayerContainer = new TextLayerContainer(area);
    addContainer("text", "", textLayerContainer, true);

    ClipLayerBase* clipLayer = area->getClipLayer();
    if(clipLayer != NULL){
        connect(clipLayer, &ClipLayerBase::boundChangeBegin, this, [=](){
            hideAll();
        });
        connect(clipLayer, &ClipLayerBase::boundChange, this, [=](QRectF bound){
            moveTo(bound);
            showAll();
        });
    }
    hide();
}
