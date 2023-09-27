#include "fadelabel.h"
#include <QGraphicsOpacityEffect>

FadeLabel::FadeLabel(const QString& text, QWidget* parent) : QLabel(text, parent)
{
    QPalette labelPal;
    QLinearGradient colorGradient = QLinearGradient(0, 0, width(), height());
    colorGradient.setSpread(QGradient::RepeatSpread);
    colorGradient.setColorAt(0, QColor("#5EFCE8"));
    colorGradient.setColorAt(0.5, QColor("#333333"));
    colorGradient.setColorAt(1, QColor("#736EFE"));
    QBrush brush(colorGradient);
    labelPal.setBrush(QPalette::ColorRole::Text, brush);

    setPalette(labelPal);
    setForegroundRole(QPalette::Text);
    QGraphicsOpacityEffect *scaleOpacityEffect = new QGraphicsOpacityEffect(this);
    scaleOpacityEffect->setOpacity(0);
    setGraphicsEffect(scaleOpacityEffect);
    scaleAnimation = new QPropertyAnimation(scaleOpacityEffect, "opacity");
    scaleAnimation->setDuration(1500);
    scaleAnimation->setStartValue(1);
    scaleAnimation->setEndValue(0);
    scaleAnimation->setEasingCurve(QEasingCurve::InOutQuad);
}

void FadeLabel::setDuration(int msec){
    scaleAnimation->setDuration(msec);
}

void FadeLabel::showAni(){
    scaleAnimation->stop();
    scaleAnimation->start();
}
