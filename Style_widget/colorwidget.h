#ifndef COLORWIDGET_H
#define COLORWIDGET_H
#include <QWidget>
#include <QGridLayout>
#include <QRegExp>
#include <QToolButton>
#include <QGridLayout>

class ColorWidget : public QWidget {
    Q_OBJECT
public:
    ColorWidget(QWidget* parent = nullptr);
    ~ColorWidget();
    void setCurrentStyle(QColor color);
    void paintEvent(QPaintEvent *event) override;
signals:
    void colorChange(const QColor& color);
private slots:
    void onDefaultClick();

private:
    void addDefaultButton(QColor color, int x, int y);

private:
    QRegExp ex;
    QColor default_color;
    QGridLayout* layout;
    QToolButton* current;
};

#endif // COLORWIDGET_H
