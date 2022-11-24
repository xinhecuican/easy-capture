#ifndef GRAPHICSVIEWPATCH_H
#define GRAPHICSVIEWPATCH_H
#include <QObject>
#include <QGraphicsView>
#include <QEvent>
class GraphicsViewPatch : public QObject {
    Q_OBJECT

public:
    explicit GraphicsViewPatch(QObject *parent = nullptr)
        : QObject(parent) {}

protected:
    bool eventFilter(QObject *watched, QEvent *event)
    {
        if (event->type() == QEvent::MouseMove) {
            QWidget *w = qobject_cast<QWidget *>(watched);
            Q_ASSERT(w);

            QGraphicsView *v = qobject_cast<QGraphicsView *>(w->parent());
            Q_ASSERT(v);

            w->update(v->rubberBandRect().marginsAdded(QMargins(1, 1, 1, 1)));
        }
        return QObject::eventFilter(watched, event);
    }
};
#endif // GRAPHICSVIEWPATCH_H
