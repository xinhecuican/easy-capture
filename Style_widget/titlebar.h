#ifndef TITLEBAR_H
#define TITLEBAR_H
#include<QWidget>
#include<QLabel>
#include<QPushButton>
#include<QToolButton>

class Titlebar : public QWidget
{
    Q_OBJECT
public:
    Titlebar(QWidget* parent=nullptr);
    void mouseDoubleClickEvent(QMouseEvent *event) override;
    bool eventFilter(QObject* object, QEvent* event)override;
signals:
    void minimize();
private:
    QLabel* icon_label;
    QLabel* name_label;
    QToolButton* min_button;
    QToolButton* max_button;
    QToolButton* close_button;
    QWidget* parent;
    QPoint begin_point;
};

#endif // TITLEBAR_H
