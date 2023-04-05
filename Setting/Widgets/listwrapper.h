#ifndef LISTWRAPPER_H
#define LISTWRAPPER_H
#include <QPushButton>

class ListWrapper : public QWidget {
    Q_OBJECT
public:
    ListWrapper(QWidget* parent=nullptr);
    void paintEvent(QPaintEvent *event) override;
};

#endif // LISTWRAPPER_H
