#ifndef DEFAULTTOOLBAR_H
#define DEFAULTTOOLBAR_H
#include "layertoolbar.h"
#include <QProcess>

class DefaultToolbar : public LayerToolBar
{
public:
    DefaultToolbar(PaintArea* area, QWidget* parent=nullptr);
private:
    void initOcr();
private:
    QProcess ocrProcess;
    QProcess showOcrResultProcess;
};

#endif // DEFAULTTOOLBAR_H
