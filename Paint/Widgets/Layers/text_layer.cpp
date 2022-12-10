#include "text_layer.h"
#include "Paint/Widgets/Panels/flow_edit_panel.h"
#include<QDebug>
#include <QApplication>

Text_layer::Text_layer(QRectF rect, QGraphicsItem* parent) : QGraphicsTextItem(parent)
{
    setTextInteractionFlags(Qt::TextEditorInteraction);
    force_focus = false;
    is_brush = false;
    setPos(rect.topLeft());
    setFont(Flow_edit_panel::instance()->default_font);
    rect_layer = new Rect_layer(this, QRectF(QPointF(0, 0), boundingRect().size()));
    rect_layer->setEnableMove(true);
    is_enable = true;
    connect(rect_layer, &Rect_layer::move, this, [=](qreal dx, qreal dy){
        moveBy(dx, dy);
        rect_layer->setPos(0, 0);
    });
    connect(document(), &QTextDocument::contentsChanged, this, [=](){
        rect_layer->setBounding(boundingRect());
    });
    requestFocus(this, this);
    setFocus();
    initFlowEditPanel();
    now_position = 0;
    is_commit_string = false;
    current_charformat.setFont(Flow_edit_panel::default_font);
    current_charformat.setForeground(Flow_edit_panel::default_color);
}

void Text_layer::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    QGraphicsTextItem::paint(painter, option, widget);
}

void Text_layer::getFocusFunc()
{
    Flow_edit_panel::instance()->show();
    force_focus = true;
}

void Text_layer::loseFocusFunc()
{
    force_focus = false;
    boundHide();
    Flow_edit_panel::instance()->hide();
    if(toPlainText() == "")
    {
        delete this;
    }
}

void Text_layer::hoverEnterEvent(QGraphicsSceneHoverEvent *event)
{
    setFocus();
    if(is_enable)
    {
        boundShow();
    }
}

void Text_layer::hoverLeaveEvent(QGraphicsSceneHoverEvent *event)
{
    if(!force_focus)
        clearFocus();
    boundHide();
}

void Text_layer::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    requestFocus(this, this);
    QGraphicsTextItem::mousePressEvent(event);
    if(now_position != textCursor().position())
    {
        onCursorPositionChange(textCursor().position());
    }
}

void Text_layer::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    if(textCursor().selectedText().size() != 0 && is_brush)
    {
        QTextCursor text_cursor(document());
        text_cursor.setPosition(textCursor().selectionStart());
        text_cursor.setPosition(textCursor().selectionEnd(), QTextCursor::KeepAnchor);
        text_cursor.setCharFormat(brush_format);
        onCursorPositionChange(textCursor().position());
    }
}

void Text_layer::focusInEvent(QFocusEvent *event)
{
    QGraphicsTextItem::focusInEvent(event);
}

void Text_layer::focusOutEvent(QFocusEvent* event)
{
    QGraphicsTextItem::focusOutEvent(event);
}

void Text_layer::keyPressEvent(QKeyEvent *event)
{
    requestFocus(this, this);
    QGraphicsTextItem::keyPressEvent(event);
    if(event->key() == Qt::Key_Right || event->key() == Qt::Key_Left)
    {
        onCursorPositionChange(textCursor().position());
    }
}

void Text_layer::inputMethodEvent(QInputMethodEvent *event)
{
    commit_position = textCursor().position();
    is_commit_string = true;
    commit_length = event->commitString().length();
    QGraphicsTextItem::inputMethodEvent(event);
}

void Text_layer::boundShow()
{
    rect_layer->show();
}

void Text_layer::boundHide()
{
    if(force_focus)
    {
        return;
    }
    rect_layer->hide();
    update();
}

void Text_layer::initFlowEditPanel()
{
    connect(Flow_edit_panel::instance(), &Flow_edit_panel::font_change, this ,[=](){
        if(force_focus)
        {
            setFocus(Qt::OtherFocusReason);
            QTextCharFormat format;
            format.setForeground(Flow_edit_panel::instance()->get_color());
            format.setFont(Flow_edit_panel::instance()->get_font());
            current_charformat = format;
        }
    });
    connect(document(), &QTextDocument::cursorPositionChanged, this, [=](const QTextCursor& cursor){
        onCursorPositionChange(cursor.position());
    });
    connect(document(), static_cast<void (QTextDocument::*)(int, int, int)>(&QTextDocument::contentsChange),
            this, [=](int position, int charsRemoved, int charsAdded){
        if(is_brush)
        {
            is_brush = false;
            return;
        }
        QTextCursor text_cursor(document());
        if(!is_commit_string)
        {
            text_cursor.setPosition(position);
            text_cursor.setPosition(position+charsAdded, QTextCursor::KeepAnchor);
        }
        else
        {
            if(charsAdded - charsRemoved == commit_length && position == 0)
            {
                text_cursor.setPosition(commit_position);
                text_cursor.setPosition(commit_position+commit_length, QTextCursor::KeepAnchor);
            }
            else
            {
                text_cursor.setPosition(position);
                text_cursor.setPosition(position+commit_length, QTextCursor::KeepAnchor);
            }
        }
        is_commit_string = false;
        text_cursor.setCharFormat(current_charformat);
    });
    connect(Flow_edit_panel::instance(), &Flow_edit_panel::text_brush, this, [=](){
        is_brush = true;
        brush_format = current_charformat;
    });
}

void Text_layer::hideNormal()
{
    QTextCursor text_cursor = textCursor();
    text_cursor.clearSelection();
    setTextCursor(text_cursor);
    rect_layer->hide();
    force_focus = false;
    is_enable = false;
    setTextInteractionFlags(Qt::NoTextInteraction);
    update();
}

void Text_layer::showNormal()
{
    is_enable = true;
    setTextInteractionFlags(Qt::TextEditorInteraction);
    update();
}

void Text_layer::onCursorPositionChange(int current)
{
    now_position = current;
    Flow_edit_panel::instance()->set_format(textCursor().charFormat().font(),
                                                textCursor().charFormat().foreground().color());
    current_charformat = textCursor().charFormat();
}

bool Text_layer::acceptFocus()
{
    return true;
}

int Text_layer::type() const
{
    return 65544;
}
