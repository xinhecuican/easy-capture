#ifndef PINWIDGET_H
#define PINWIDGET_H
#include <QWidget>
#include <QMenu>
#include <QLabel>
#include <QGraphicsOpacityEffect>
#include <QPropertyAnimation>
#include "Style_widget/fadelabel.h"

class PinWidget : public QWidget
{
    Q_OBJECT
public:
    PinWidget(QImage image, QRect bound, QWidget* parent=nullptr);
    void paintEvent(QPaintEvent *event) override;
    void contextMenuEvent(QContextMenuEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void focusInEvent(QFocusEvent *event) override;
    void focusOutEvent(QFocusEvent *event) override;
    void wheelEvent(QWheelEvent *event) override;
signals:
    void closeNow();
    void closeAll();
    void editNow(QImage image, QRect bound);
private:
    void initMenu();
    QImage image;
    QRect bound;
    QPoint beginPoint;
    QPoint topLeftPoint;
    QRect screenGeometry;
    bool isPress;
    const int shadowWidth = 10;
    QColor shadowColor;
    QColor normalColor;
    QMenu* menu;
    FadeLabel* scaleLabel;
    int currentWidth;
    int currentHeight;
    int scale=100;
};

#endif // PINWIDGET_H
