#ifndef _BUBBLETIPSWIDGET_H_
#define _BUBBLETIPSWIDGET_H_

#include <QWidget>
#include <QTimer>
#include <QLabel>

QT_BEGIN_NAMESPACE
namespace Ui {
class BubbleTipsWidget;
}
QT_END_NAMESPACE

// 三角形的高度
#define DEF_TRIANGLE_HEIGHT 10
// 透明宽度
#define TRANSPARENT_LENGTH 10
// 文字左边距
#define LEFT_MARGIN 10
// 文字上边距
#define TOP_MARGIN 10

enum class DIRECT {
    DIRECT_LEFT = 1,
    DIRECT_TOP,
    DIRECT_RIGHT,
    DIRECT_BOTTOM
};

class BubbleTipsWidget : public QWidget {
    Q_OBJECT
public:
    explicit BubbleTipsWidget(QWidget *parent = nullptr);
    ~BubbleTipsWidget();

    void setBackColor(int r, int g, int b, int a = 255);
    // 设置三角方向(左上右下)，位置系数(宽度 * 系数)
    void setDirect(DIRECT direct = DIRECT::DIRECT_TOP, double size = 0.75);
    void setContent(const QString &content, QColor color = {});
    inline void setContent(QWidget* widget);
    void addContent(QString content, QColor color = QColor());
    void addContents(QStringList content, QColor color = QColor());
    void setFixContent(const QString content, QColor color = {});
    void clear();
    void setLeftTopMargin(int leftMargin = LEFT_MARGIN, int topMargin = TOP_MARGIN);
    void hideEvent(QHideEvent *event) override;
    void showEvent(QShowEvent *event) override;
    void setFix(bool isFix);
    void setShowHideButton(bool show);
public slots:
    void contentChangeFunc();
signals:
    void hideButtonTrigger(bool enable);
protected:
    void paintEvent(QPaintEvent *event)override;

private:
    void setContentWidget(QWidget* widget);
    QLabel* initLabelContent(const QString &content, QColor color = {});
    Ui::BubbleTipsWidget *ui;
    QColor          m_backColor  {255, 0, 255, 255};
    DIRECT          m_direct;

    double          m_posSize    {0.75};
    int             m_leftMargin {LEFT_MARGIN};
    int             m_topMargin  {TOP_MARGIN};
    struct ContentType {
        QString text;
        QColor color;
    };
    int currentIndex;
    int loopTime;
    QList<QWidget*> contents;
    QTimer* contentChangeTimer;
    QWidget* currentWidget;
    bool isFixContent;
    bool isShowHideButton;
};
#endif // _BUBBLETIPSWIDGET_H_

