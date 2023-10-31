#include "bubbletipswidget.h"
#include "ui_bubbletipswidget.h"
#include <QPainter>
#include <QHideEvent>
#include "Helper/mstring.h"

BubbleTipsWidget::BubbleTipsWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::BubbleTipsWidget) {
    ui->setupUi(this);
    ui->hideButton->hide();
    currentWidget = NULL;
    connect(ui->hideButton, &QRadioButton::toggled, this, [=](bool checked){
        emit hideButtonTrigger(checked);
    });
    isShowHideButton = false;
    setWindowFlags(Qt::FramelessWindowHint | Qt::Tool);
    setAttribute(Qt::WA_TranslucentBackground);
    setBackColor(146, 219, 176, 130);
    setDirect(DIRECT::DIRECT_RIGHT, 1);

    QFont f;
    f.setFamily("Microsoft YaHei UI");
    f.setPixelSize(18);

    loopTime = 3000;
    currentIndex = 0;
    contentChangeTimer = new QTimer(this);
    connect(contentChangeTimer, &QTimer::timeout, this, &BubbleTipsWidget::contentChangeFunc);
//    setLeftTopMargin();
}

BubbleTipsWidget::~BubbleTipsWidget() {
    delete ui;
}

void BubbleTipsWidget::addContent(QString content, QColor color) {
    QLabel* label = initLabelContent(content, color);
    contents.append(label);
    setContent(label);
    if(!contentChangeTimer->isActive())
        contentChangeTimer->start(loopTime);
}

void BubbleTipsWidget::addContents(QStringList contents, QColor color) {
    for(QString content : contents) {
        QLabel* label = initLabelContent(content, color);
        this->contents.append(label);
    }
}

inline void BubbleTipsWidget::setContent(QWidget* widget){
    setContentWidget(widget);
    adjustSize();
    update();
}

void BubbleTipsWidget::clear() {
    contents.clear();
}

void BubbleTipsWidget::setBackColor(int r, int g, int b, int a) {
    m_backColor = QColor(r, g, b, a);
}

void BubbleTipsWidget::setDirect(DIRECT direct, double size) {
    m_direct = direct;
    m_posSize = size;
}

void BubbleTipsWidget::setContent(const QString &content, QColor color) {
    QLabel* label = initLabelContent(content, color);
    contents.clear();
    contents.append(label);
    setContent(label);
}

void BubbleTipsWidget::hideEvent(QHideEvent *event) {
    QWidget::hideEvent(event);
    if(contentChangeTimer->isActive())
        contentChangeTimer->stop();
}

void BubbleTipsWidget::showEvent(QShowEvent *event) {
    QWidget::showEvent(event);
    if(!contentChangeTimer->isActive()){
        contentChangeFunc();
        contentChangeTimer->start(loopTime);
    }
}

void BubbleTipsWidget::setLeftTopMargin(int leftMargin, int topMargin) {
    m_leftMargin = leftMargin;
    m_topMargin = topMargin;
    this->setContentsMargins(m_leftMargin + TRANSPARENT_LENGTH,
                             m_topMargin + TRANSPARENT_LENGTH,
                             m_leftMargin + TRANSPARENT_LENGTH,
                             m_topMargin + TRANSPARENT_LENGTH);
}

void BubbleTipsWidget::paintEvent(QPaintEvent *event) {
    Q_UNUSED(event)

    QPainter painter(this);
    painter.setRenderHints(QPainter::Antialiasing);


    painter.setPen(Qt::NoPen);
    painter.setBrush(m_backColor);

    //相对于当前布局的起点坐标
    painter.drawRoundedRect(TRANSPARENT_LENGTH, TRANSPARENT_LENGTH,
                            width() - TRANSPARENT_LENGTH * 2,
                            height() - TRANSPARENT_LENGTH * 2, 4, 4);
}

void BubbleTipsWidget::setFixContent(const QString content, QColor color) {
    setFix(true);
    setContent(content, color);
}

void BubbleTipsWidget::setFix(bool isFix) {
    if(isFix) {
        isFixContent = true;
        contentChangeTimer->stop();
    } else {
        isFixContent = false;
        contentChangeFunc();
        contentChangeTimer->start(loopTime);
    }
}

void BubbleTipsWidget::contentChangeFunc(){
    if(contents.size() == 0)
        return;
    if(contents.size() <= currentIndex)
        currentIndex = 0;
    else
        currentIndex = (currentIndex + 1) % contents.size();
    setContent(contents[currentIndex]);
}

void BubbleTipsWidget::setShowHideButton(bool show){
    if(show)
        ui->hideButton->show();
    else
        ui->hideButton->hide();
}

QLabel* BubbleTipsWidget::initLabelContent(const QString &content, QColor color){
    QLabel* label = new QLabel(this);
    QFont f;
    f.setFamily("Microsoft YaHei UI");
    f.setPixelSize(18);
    label->setFont(f);
    label->setText(MString::search(content));
    label->setStyleSheet(QString("color: rgb(%1, %2, %3)")
                                 .arg(color.red())
                                 .arg(color.green())
                                 .arg(color.blue()));
    label->hide();
    return label;
}

void BubbleTipsWidget::setContentWidget(QWidget* widget){
    if(currentWidget == NULL || currentWidget != widget){
        if(currentWidget != NULL)
            currentWidget->hide();
        widget->show();
        ui->contentLayout->removeWidget(currentWidget);
        ui->contentLayout->addWidget(widget);
        currentWidget = widget;
    }
}
