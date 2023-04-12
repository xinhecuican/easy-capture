#ifndef TEXT_LAYER_H
#define TEXT_LAYER_H
#include<QWidget>
#include<QLineEdit>
#include<QTextEdit>
#include<QPlainTextEdit>
#include "Paint/Data/Ilayer.h"
#include "Helper/plist.h"
#include "Paint/Data/stretch_button.h"
#include "Paint/Data/Common.h"
#include<QGraphicsTextItem>
#include "baselayer.h"
#include "rect_layer.h"

class Text_layer : public QGraphicsTextItem, public BaseLayer {
    Q_OBJECT
public:
    Text_layer(QRectF rect, QGraphicsItem* parent=nullptr);
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;
    void getFocusFunc() override;
    void loseFocusFunc() override;
    void hoverEnterEvent(QGraphicsSceneHoverEvent *event) override;
    void hoverLeaveEvent(QGraphicsSceneHoverEvent *event) override;
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;
    void focusInEvent(QFocusEvent *event) override;
    void focusOutEvent(QFocusEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;
    void inputMethodEvent(QInputMethodEvent* event) override;
    int type() const override;
    Q_INVOKABLE void hideNormal() override;
    Q_INVOKABLE void showNormal() override;
    Q_INVOKABLE bool acceptFocus() override;
signals:
    void requestFocus(BaseLayer* object, QGraphicsObject* object2);
private:
    void boundShow();
    void boundHide();
    void initFlowEditPanel();
    void onCursorPositionChange(int current);
    QRectF rect;
    RectLayer* rect_layer;
    bool force_focus;
    bool is_enable;
    bool is_commit_string;
    bool is_brush;
    int commit_length;
    int commit_position;
    int now_position;
    QTextCharFormat brush_format;
    QTextCharFormat current_charformat;
};

#endif // TEXT_LAYER_H
