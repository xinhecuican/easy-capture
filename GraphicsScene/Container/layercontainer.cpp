#include "layercontainer.h"
#include "../paintarea.h"

LayerContainer::LayerContainer(PaintArea* area):
    QObject(area),
    area(area),
    widget(NULL){

}


void LayerContainer::addWidget(QWidget *widget){
    this->widget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    this->widget->layout()->addWidget(widget);
}

bool LayerContainer::initWidget(QWidget* parent){
    if(widget == NULL){
        widget = new QWidget(parent);
        QHBoxLayout* layout = new QHBoxLayout();
        layout->setSpacing(3);
        layout->setContentsMargins(3, 2, 3, 2);
        widget->setLayout(layout);
        return false;
    }
    return true;
}
