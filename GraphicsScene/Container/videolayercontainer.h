#ifndef VIDEOLAYERCONTAINER_H
#define VIDEOLAYERCONTAINER_H
#include "layercontainer.h"
#include "ecvideocapture.h"

class VideoLayerContainer : public LayerContainer
{
public:
    VideoLayerContainer(PaintArea* area);
    QWidget * onValid(QWidget *widgetParent) override;
private:
    RecordInfo info;
    QString name;
    QString suffix;
};

#endif // VIDEOLAYERCONTAINER_H
