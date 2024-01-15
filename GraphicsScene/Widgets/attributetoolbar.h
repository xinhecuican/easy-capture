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
    void addWidget(QWidget* widget);
    void showWidget(QWidget* widget);
    void paintEvent(QPaintEvent *event) override;
    void removeAll();
    void setAttriLayout(QLayout* layout);
    void hideAll();
private:
    bool containWidget(QWidget* widget);
    QHBoxLayout* layout;
    QList<QWidget*> widgets;
};

#endif // ATTRIBUTETOOLBAR_H
