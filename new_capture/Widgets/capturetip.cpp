#include "capturetip.h"
#include <QVBoxLayout>
#include <QLabel>
#include "Helper/imagehelper.h"

CaptureTip::CaptureTip(QWidget* parent) : QWidget(parent)
{
    QVBoxLayout* layout = new QVBoxLayout();
    QHBoxLayout* rightButtonLayout = new QHBoxLayout();
    QLabel* rightButtonLabel = new QLabel(this);
    QLabel* rightButtonIcon = new QLabel(this);
    rightButtonIcon->setPixmap(ImageHelper::getPixmap("mouse_right", 16, 16));
    rightButtonLabel->setText("退出截图");
    rightButtonLabel->setIndent(4);
    rightButtonLayout->addWidget(rightButtonIcon);
    rightButtonLayout->addWidget(rightButtonLabel);

    QHBoxLayout* middleButtonLayout = new QHBoxLayout();
    QLabel* middleButtonIcon = new QLabel(this);
    QLabel* middleButtonLabel = new QLabel(this);
    middleButtonIcon->setPixmap(ImageHelper::getPixmap("mouse_middle", 16, 16));
    middleButtonLabel->setText("复制截图到剪切板");
    middleButtonLabel->setIndent(4);
    middleButtonLayout->addWidget(middleButtonIcon);
    middleButtonLayout->addWidget(middleButtonLabel);

    layout->addLayout(rightButtonLayout);
    layout->addLayout(middleButtonLayout);
    setLayout(layout);
}
