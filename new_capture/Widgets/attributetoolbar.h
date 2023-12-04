#ifndef ATTRIBUTETOOLBAR_H
#define ATTRIBUTETOOLBAR_H
#include <QWidget>
#include <QHBoxLayout>
#include <QToolBar>

class AttributeToolbar : public QWidget
{
    Q_OBJECT
public:
    AttributeToolbar(QWidget* parent=nullptr);
    void add(QWidget* widget);
    void paintEvent(QPaintEvent *event) override;
    void removeAll();
private:
    QHBoxLayout* layout;
};

#endif // ATTRIBUTETOOLBAR_H
