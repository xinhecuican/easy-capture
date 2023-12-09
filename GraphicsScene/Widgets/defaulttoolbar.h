#ifndef DEFAULTTOOLBAR_H
#define DEFAULTTOOLBAR_H
#include "layertoolbar.h"

class DefaultToolbar : public LayerToolBar
{
public:
    DefaultToolbar(PaintArea* area, QWidget* parent=nullptr);
};

#endif // DEFAULTTOOLBAR_H
