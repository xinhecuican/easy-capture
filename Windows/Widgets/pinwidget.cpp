#include "pinwidget.h"
#include <QPainter>
#include <QContextMenuEvent>
#include <QMenu>
#include "Helper/PaintHelper.h"
#include "Helper/imagehelper.h"
#include <QDebug>
#include <QFrame>
#include <QApplication>
#include <QClipboard>
#include <QFileDialog>
#include "Helper/mstring.h"
#include <QPropertyAnimation>
#include <QPalette>

PinWidget::PinWidget(QImage image, QRect bound, QWidget* parent) : QWidget(parent),
    image(image),
    bound(bound),
    screenGeometry(ImageHelper::getCurrentGeometry()),
    isPress(false),
    shadowColor(QColor(50, 50, 50, 50)),
    currentWidth(bound.width()),
    currentHeight(bound.height())

{
    scaleLabel = new FadeLabel("1000%", this);
    scaleLabel->setFont(QFont("Roman", 20, 75));
    normalColor = shadowColor;


    QRect screenBound = bound;
    screenBound.setTopLeft(bound.topLeft() - QPoint(shadowWidth, shadowWidth));
    screenBound.setBottomRight(bound.bottomRight() + QPoint(shadowWidth, shadowWidth));
    setGeometry(screenBound);
    setMouseTracking(true);
    setFocusPolicy(Qt::ClickFocus);
    initMenu();
}

void PinWidget::paintEvent(QPaintEvent *event){
    QPainter painter(this);
    qreal scaleFactor = (qreal)(scale) / 100.0;
    painter.scale(scaleFactor, scaleFactor);
    painter.drawImage(shadowWidth, shadowWidth, image);
    PaintHelper::paintShadow(&painter, QRect(shadowWidth, shadowWidth, bound.width(), bound.height()), shadowWidth, shadowColor);
}

void PinWidget::contextMenuEvent(QContextMenuEvent *event){
    menu->exec(this->mapToGlobal(event->pos()));
}

void PinWidget::mousePressEvent(QMouseEvent *event){
    if(event->button() == Qt::LeftButton){
        isPress = true;
        beginPoint = event->globalPos();
        topLeftPoint = frameGeometry().topLeft();
    }
}

void PinWidget::mouseMoveEvent(QMouseEvent *event){
    if(isPress){
        QPoint point = topLeftPoint + event->globalPos() - beginPoint;
        move(point);
    }
}

void PinWidget::mouseReleaseEvent(QMouseEvent *event){
    isPress = false;
}

void PinWidget::focusInEvent(QFocusEvent *event){
    shadowColor = QColor(38, 78, 119, 90);
}

void PinWidget::focusOutEvent(QFocusEvent *event){
    shadowColor = normalColor;
}

void PinWidget::wheelEvent(QWheelEvent *event){
    if(event->delta() > 0){
        scale += 10;
    }
    else if(scale > 20){
        scale -= 10;
    }
    qreal scaleFactor = (qreal)(scale) / 100.0;
    qreal beforeWidth = currentWidth;
    qreal beforeHeight = currentHeight;
    currentWidth = bound.width() * scaleFactor;
    currentHeight = bound.height() * scaleFactor;
    qreal deltaWidth = currentWidth - beforeWidth;
    qreal deltaHeight = currentHeight - beforeHeight;
    QPoint centerPos = event->pos();
    move(pos() - QPoint(deltaWidth * centerPos.x() / beforeWidth, deltaHeight * (qreal)(centerPos.y()) / beforeHeight));
    resize(ImageHelper::getCurrentGeometry().size());
    scaleLabel->move(event->pos());
    scaleLabel->setText(QString::number(scale) + "%");
    scaleLabel->showAni();
}

void PinWidget::initMenu(){
    menu = new QMenu(this);
    QAction* copy = new QAction(MString::search("复制"), this);
    connect(copy, &QAction::triggered, this, [=](){
        QClipboard *clip=QApplication::clipboard();
        clip->setImage(image);
    });
    QAction* save = new QAction(MString::search("{pJqTHhEQdb}保存"), this);
    connect(save, &QAction::triggered, this, [=](){
        QString file_name = QFileDialog::getSaveFileName(this,
                                                         "保存",
                                                         "",
                                                         "图片(*.bmp *.jpg *.jpeg *.png);;所有文件(*)");
        if(file_name != "") {
            image.save(file_name);
        }
    });
    QAction* close = new QAction(MString::search("{FuBCvgW4BE}关闭"), this);
    connect(close, &QAction::triggered, this, [=](){
        emit closeNow();
    });
    QAction* closeAllAction = new QAction(MString::search("{hpKdt8TR1o}关闭全部"), this);
    connect(closeAllAction, &QAction::triggered, this, [=](){
        emit closeAll();
    });
    menu->addAction(copy);
    menu->addAction(save);
    menu->addAction(close);
    menu->addAction(closeAllAction);
}
