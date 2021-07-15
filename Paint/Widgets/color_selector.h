#ifndef COLOR_SELECTOR_H
#define COLOR_SELECTOR_H
#include<QMainWindow>
#include<QPaintEvent>
#include<QMouseEvent>
#include<QImage>
#include<QRgb>

class Color_selector : public QMainWindow
{
    Q_OBJECT
public:
    Color_selector();
    ~Color_selector();
    Color_selector(QWidget* parent);
    void paintEvent(QPaintEvent* event) override;
    void mousePressEvent(QMouseEvent* event)override;
    void mouseMoveEvent(QMouseEvent* event)override;
signals:
    void color_select(QColor color);

private:
    bool mouse_press;
    QColor selected_color;
    QImage region;
    QPoint region_pos;
    int window_w;
    int window_h;
    QPoint pos;
    void decide_region_position(QPoint mouse_pos);
    const QColor src = QColor(0, 0, 0, 1);
    QColor cal_dest_color(QColor res);
};

#endif // COLOR_SELECTOR_H
