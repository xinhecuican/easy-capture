#include "rotate_view.h"
#include<QResizeEvent>

Rotate_view::Rotate_view(QWidget* parent) : QGraphicsView(parent)
{
    setFrameStyle(QFrame::NoFrame);
    setStyleSheet("background: transparent");
    scene = new QGraphicsScene(this);
    scene->addRect(parent->geometry());
    setGeometry(parent->geometry());
    setScene(scene);
}

void Rotate_view::resizeEvent(QResizeEvent *event)
{
    setSceneRect(QRect(QPoint(0, 0), event->size()));

}

QGraphicsProxyWidget* Rotate_view::add_widget(QWidget* widget)
{
    return scene->addWidget(widget);
}
