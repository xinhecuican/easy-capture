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
    bound(ImageHelper::getCurrentGeometry()),
    beforeId(0)
{
    group->setExclusive(true);
    connect(group, static_cast<void (QButtonGroup::*)(int)>(&QButtonGroup::buttonClicked), this, &LayerToolBar::onGroupClick);
    setWindowFlags(windowFlags() | Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
    setWindowFlag(Qt::WindowSystemMenuHint, false);
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

void LayerToolBar::addContainer(QToolButton* button, std::function<void()>const& f, std::function<void()>const& leavef, bool exclude){
    if(!exclude){
        connect(button, &QToolButton::clicked, this, f);
    }
    else{
        button->setCheckable(true);
        button->setChecked(false);
        group->addButton(button, groupId);
        validF[groupId] = f;
        invalidF[groupId] = leavef;
        groupId++;
        containers.append(NULL);
    }
    addWidget(button);
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
    if(left + maxWidth > bound.width()) {
        left = bound.width() - maxWidth;
    }
    if(left < 0) {
        left = 0;
    }
    if(top + bar_height + attributeBarHeight > bound.height()) {
        top = bound.height() - bar_height - attributeBarHeight;
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
    execInvalid(beforeId);
    execValid(id);
    beforeId = id;
    moveTo(mask);
}

void LayerToolBar::simulateClick(int id){
    group->button(id)->setChecked(true);
    onGroupClick(id);
}

void LayerToolBar::hideAll(){
    hide();
    attributeBar->hide();
}

void LayerToolBar::showAll(){
    show();
    attributeBar->show();
}

void LayerToolBar::execValid(int id){
    if(containers[id] != NULL){
        area->setContainer(containers[id]);
        QWidget* widget = containers[id]->onValid(attributeBar);
        attributeBar->showWidget(widget);
    }
    else{
        auto f = validF.find(id);
        if(f != validF.end()){
            f.value()();
        }
        area->setContainer(NULL);
        attributeBar->adjustSize();
    }
}

void LayerToolBar::execInvalid(int id){
    if(containers[id] != NULL){
        area->setContainer(containers[id]);
        containers[id]->onInvalid();
    }
    else{
        auto f = invalidF.find(id);
        if(f != invalidF.end()){
            f.value()();
        }
        attributeBar->adjustSize();
    }
}

LayerContainer* LayerToolBar::getCurrentGroupContainer(){
    if(containers.size() == 0){
        return NULL;
    }
    return containers[group->checkedId()];
}

void LayerToolBar::reset(){
    if(area != NULL){
        area->reset();
    }
    for(auto container : containers){
        if(container != NULL) container->reset();
    }
    bound = ImageHelper::getCurrentGeometry();
}
