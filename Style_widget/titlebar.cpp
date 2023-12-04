#include "titlebar.h"
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include<QHBoxLayout>
#include<QMouseEvent>
#include<QDebug>
#include "../Helper/imagehelper.h"

Titlebar::Titlebar(QWidget* parent) : QWidget(parent)
{
    this->parent = parent;
    icon_label = new QLabel(this);
    name_label = new QLabel(this);
    min_button = new QToolButton(this);
    max_button = new QToolButton(this);
    close_button = new QToolButton(this);
    setFixedHeight(30);

    icon_label->setScaledContents(true);
    icon_label->setFixedSize(30, 30);
    name_label->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

    min_button->setIcon(ImageHelper::getIcon("minimize"));
    min_button->setMinimumSize(30, 30);
    min_button->setToolTip("最小化");
    min_button->setStyleSheet("QToolButton{border:none;}\n"
                "QToolButton:hover {\n"
                              "background-color: rgb(233, 233, 233);\n"
                          "}\n"
                          "QToolButton:pressed {\n"
                              "background-color: rgb(150, 150, 150);\n"
                          "}");
    connect(min_button, &QPushButton::clicked, this, [=](){
        parent->showMinimized();
        emit minimize();
    });

    max_button->setIcon(ImageHelper::getIcon("maximize"));
    max_button->setToolTip("最大化");
    max_button->setMinimumSize(30, 30);
    max_button->setStyleSheet("QToolButton{border:none;}\n"
                              "QToolButton:hover {\n"
                                            "background-color: rgb(233, 233, 233);\n"
                                        "}\n"
                                        "QToolButton:pressed {\n"
                                            "background-color: rgb(150, 150, 150);\n"
                                        "}");
    connect(max_button, &QPushButton::clicked, this, [=](){
        parent->isMaximized() ? parent->showNormal() : parent->showMaximized();
    });
    QPixmap pix = QPixmap(":/image/close.svg");
    pix = pix.scaled(30, 30);
    close_button->setIcon(QIcon(pix));
    close_button->setToolTip("关闭");
    close_button->setMinimumSize(30, 30);
    close_button->setStyleSheet("QToolButton{border:none;}\n"
                                "QToolButton:hover {\n"
                                              "background-color: rgb(232, 17, 36);\n"
                                          "}\n"
                                          "QToolButton:pressed {\n"
                                              "background-color: rgba(232, 17, 36, 0.7);\n"
                                          "}");
    connect(close_button, &QPushButton::clicked, this, [=](){
        parent->close();
    });

    QHBoxLayout* base_layout = new QHBoxLayout(this);
    base_layout->addWidget(icon_label);
    base_layout->addSpacing(5);
    base_layout->addWidget(name_label, 0, Qt::AlignCenter);
    base_layout->addStretch();
    base_layout->addWidget(min_button);
    base_layout->addWidget(max_button);
    base_layout->addWidget(close_button);
    base_layout->addSpacing(0);
    base_layout->setContentsMargins(5, 0, 0, 0);

}

void Titlebar::mouseDoubleClickEvent(QMouseEvent *event)
{
    emit max_button->clicked();
}

bool Titlebar::eventFilter(QObject *object, QEvent *event)
{
    switch ( event->type() ) //判断发生事件的类型
    {
    case QEvent::WindowTitleChange: //窗口标题改变事件
    {
        QWidget *pWidget = qobject_cast<QWidget *>(object); //获得发生事件的窗口对象
        if (pWidget)
        {
            //窗体标题改变，则标题栏标题也随之改变
            name_label->setText(pWidget->windowTitle());
            return true;
        }
    }
        break;

    case QEvent::WindowIconChange: //窗口图标改变事件
    {
        QWidget *pWidget = qobject_cast<QWidget *>(object);
        if (pWidget)
        {
            //窗体图标改变，则标题栏图标也随之改变
            QIcon icon = pWidget->windowIcon();
            icon_label->setPixmap(icon.pixmap(icon_label->size()));
            return true;
        }
    }
        break;

    case QEvent::Resize:
        return true;

    default:
        return QWidget::eventFilter(object, event);
    }

    return QWidget::eventFilter(object, event);

}
