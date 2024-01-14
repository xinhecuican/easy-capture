#ifndef PAINTTOOLBAR_H
#define PAINTTOOLBAR_H
#include "layertoolbar.h"
#include <QProcess>
#include "../Container/paintlayercontainer.h"

class PaintToolbar : public LayerToolBar
{
public:
    PaintToolbar(PaintArea* area, QWidget* parent=nullptr);
    QWidget* getAttributeBar();
    void reset();
private:
    void initOcr();
private:
    QProcess ocrProcess;
    QProcess showOcrResultProcess;
    PaintLayerContainer* paintLayerContainer;
};

#endif // PAINTTOOLBAR_H
