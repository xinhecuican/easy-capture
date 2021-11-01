#ifndef ROTATE_VIEW_H
#define ROTATE_VIEW_H
#include<QGraphicsView>
#include<QGraphicsScene>
#include<QGraphicsProxyWidget>

class Rotate_view : public QGraphicsView
{
public:
    Rotate_view(QWidget* parent);
    QGraphicsProxyWidget* add_widget(QWidget* widget);
    void resizeEvent(QResizeEvent *event) override;

private:
    QGraphicsScene* scene;
};

#endif // ROTATE_VIEW_H
