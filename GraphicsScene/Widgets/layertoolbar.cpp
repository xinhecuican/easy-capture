#include "layertoolbar.h"
#include <QButtonGroup>
#include "../../Helper/mstring.h"
#include "../../Helper/imagehelper.h"
#include <QDebug>

LayerToolBar::LayerToolBar(PaintArea* area, QWidget* parent) :
    QToolBar(parent),
    area(area),
    group(new QButtonGroup(this)),
    groupId(0),
    attributeBar(new AttributeToolbar(parent)),
    bound(ImageHelper::getCurrentGeometry())
{
    group->setExclusive(true);
    connect(group, static_cast<void (QButtonGroup::*)(int)>(&QButtonGroup::buttonClicked), this, &LayerToolBar::onGroupClick);
}

void LayerToolBar::addContainer(QToolButton* button, LayerContainer* container, bool exclude){
    if(!exclude){
        connect(button, &QToolButton::clicked, this, [=](){
            area->setContainer(container);
            QWidget* widget = container->onValid(attributeBar);
            attributeBar->addWidget(widget);
            moveTo(mask);
        });
    }
    else{
        button->setCheckable(true);
        button->setChecked(false);
        group->addButton(button, groupId);
        groupId++;
        containers.append(container);
    }
    addWidget(button);
    adjustSize();
}

void LayerToolBar::addContainer(const QString &icon,
                                const QString &tip,
                                LayerContainer *container,
                                bool exclude){
    QToolButton* toolButton = new QToolButton(this);
    toolButton->setToolTip(MString::search(tip));
    toolButton->setIcon(ImageHelper::getIcon(icon));
    addContainer(toolButton, container, exclude);
}

void LayerToolBar::moveTo(const QRectF& mask){
    this->mask = mask;
    if(!isVisible())
        show();
    if(!attributeBar->isVisible())
        attributeBar->show();

    if(group->checkedButton() == NULL) {
        qInfo() << "checked button NULL";
        return;
    }
    int bar_height = height();
    int bar_width = width();
    QRectF mask_bound = mask;
    int left = mask_bound.right() - bar_width;
    int top = mask_bound.bottom();

    int attributeBarWidth = attributeBar->width();
    int attributeBarHeight = attributeBar->height();
    int maxWidth = group->checkedButton()->pos().x() + attributeBarWidth > bar_width ? group->checkedButton()->pos().x() + attributeBarWidth : bar_width;
    if(left + maxWidth > bound.right()) {
        left = bound.right() - maxWidth;
    }
    if(left < 0) {
        left = 0;
    }
    if(top + bar_height + attributeBarHeight > bound.bottom()) {
        top = bound.bottom() - bar_height - attributeBarHeight;
    }
    if(top < 0) {
        top = 0;
    }
    if(left == 0 && top == 0){
        QPoint toolbarPos = mapToGlobal(QPoint(left, top));
        left = toolbarPos.x();
        top = toolbarPos.y();
    }
    move(left, top);
    attributeBar->move(left + group->checkedButton()->pos().x(), top + bar_height);
}

void LayerToolBar::setContainer(LayerContainer *container){
    for(int i=0; i<containers.size(); i++){
        if(containers[i] == container){
            group->button(i)->setChecked(true);
            onGroupClick(i);
            break;
        }
    }
}

void LayerToolBar::onGroupClick(int id){
    area->setContainer(containers[id]);
    QWidget* widget = containers[id]->onValid(attributeBar);
    attributeBar->showWidget(widget);
    moveTo(mask);
}
