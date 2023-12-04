#ifndef TEXTITEM_H
#define TEXTITEM_H
#include <QGraphicsTextItem>
#include "../Layer/rectlayer.h"
#include <QTextCharFormat>

class TextItem : public QGraphicsTextItem
{
    Q_OBJECT
public:
    TextItem(const QPointF& rect,
             QGraphicsItem* parent=nullptr);
    void getFocus();
    void loseFocus();
    void hoverEnterEvent(QGraphicsSceneHoverEvent *event) override;
    void hoverLeaveEvent(QGraphicsSceneHoverEvent *event) override;
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;
    void focusInEvent(QFocusEvent *event) override;
    void focusOutEvent(QFocusEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;
    void inputMethodEvent(QInputMethodEvent* event) override;
    void setEnable(bool enable);
    void setFont(const QFont& font);
    void setColor(QColor color);
    void setBrush(bool enable);

signals:
    void requestFocus();
    void formatChange(const QFont& font, const QBrush& brush);

private:
    void boundShow();
    void boundHide();
    void initFlowEditPanel();
    void onCursorPositionChange(int current);
private:

    QRectF rect;
    RectLayer* rectLayer;
    bool forceFocus;
    bool mEnable;
    bool commitString;
    bool isBrush;
    int commitLen;
    int commitPos;
    int nowPos;
    QTextCharFormat brushFormat;
    QTextCharFormat currentFormat;
};

#endif // TEXTITEM_H
