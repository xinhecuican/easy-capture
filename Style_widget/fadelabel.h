#ifndef FADELABEL_H
#define FADELABEL_H
#include <QLabel>
#include <QPropertyAnimation>

class FadeLabel : public QLabel
{
public:
    FadeLabel(const QString& text, QWidget* parent=nullptr);
    void setDuration(int msec);
    void showAni();
private:
    QPropertyAnimation *scaleAnimation;
};

#endif // FADELABEL_H
