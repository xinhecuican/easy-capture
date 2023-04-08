#ifndef COLORWIDGET_H
#define COLORWIDGET_H
#include <QWidget>
#include <QGridLayout>
#include <QRegExp>
#include <QToolButton>

QT_BEGIN_NAMESPACE
namespace Ui {
class ColorWidget;
}
QT_END_NAMESPACE

class ColorWidget : public QWidget {
    Q_OBJECT
public:
    ColorWidget(QWidget* parent = nullptr);
    ~ColorWidget();
    void setCurrentStyle(QColor color);
    void paintEvent(QPaintEvent *event) override;
private slots:
    void onDefaultClick();

private:
    QRegExp ex;
    QColor default_color;
    Ui::ColorWidget* ui;
};

#endif // COLORWIDGET_H
