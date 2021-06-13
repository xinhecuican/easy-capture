#ifndef STRETCH_POINT_H
#define STRETCH_POINT_H

#include <QWidget>
#include<QPushButton>
#include<functional>

class Capture_area;
class Stretch_point : public QPushButton
{
public:
    Stretch_point();
    explicit Stretch_point(Capture_area* region, QPoint point);
    explicit Stretch_point(Capture_area* region, QWidget* parent, QPoint point);
    Stretch_point(Capture_area* region, QWidget* parent);
    ~Stretch_point();
    //void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void change_to_position(QPoint &point);
    void translate(int dx, int dy);//移动

    void set_neigh(Stretch_point* point);
    void set_node(QPoint point);
    void set_index(int index);
    void set_pos(int pos);

    QList<int> get_index();
    int getx();
    int gety();
    QPoint get_node();
    Stretch_point* next;

protected:
    void on_neigh_move(bool is_hneigh, int sum);
    QPoint get_angle_point();//获得对角线上的点，不能超过对角线
private:
    int position;
    Capture_area* region;
    QWidget* parent;
    int index[7];//在polygon中的位置
    int index_count;
    Stretch_point* h_neigh;
    Stretch_point* v_neigh;
    QPoint point;
    QPoint begin_point;
    void set_style(QString name="custom_button.qss");
};

#endif // STRETCH_POINT_H
