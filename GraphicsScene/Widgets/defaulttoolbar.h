#ifndef DEFAULTTOOLBAR_H
#define DEFAULTTOOLBAR_H
#include "layertoolbar.h"
#include <QProcess>
#include "../Container/masklayercontainer.h"

class DefaultToolbar : public LayerToolBar
{
public:
    DefaultToolbar(PaintArea* area, QWidget* parent=nullptr);
    QWidget* getAttributeBar();
    void reset() override;
private:
    void initOcr();
    bool mousePressHook(Qt::MouseButton button);
private:
    QProcess ocrProcess;
    QProcess showOcrResultProcess;
    MaskLayerContainer* maskLayerContainer;
};

#endif // DEFAULTTOOLBAR_H
