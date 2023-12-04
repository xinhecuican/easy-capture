#include "textitem.h"
#include "../../Manager/uimanager.h"
#include <QTextDocument>
#include <QTextCursor>
#include <QKeyEvent>

TextItem::TextItem(const QPointF& pos, QGraphicsItem* parent) : QGraphicsTextItem(parent),
    forceFocus(false),
    mEnable(true),
    commitString(false),
    isBrush(false),
    nowPos(0)
{
    setTextInteractionFlags(Qt::TextEditorInteraction);
    setPos(pos);
    rectLayer = new RectLayer(QRectF(QPointF(0, 0), boundingRect().size()), "", NULL, this, false);
    rectLayer->setEnable(true);
    PaintData rectData;
    rectData.color = QColor(0, 0, 0);
    rectData.width = 1;
    rectData.joinStyle = Qt::RoundJoin;
    rectLayer->setStyle(rectData);
    connect(rectLayer, &RectLayer::move, this, [=](qreal dx, qreal dy) {
        moveBy(dx, dy);
        rectLayer->setPos(0, 0);
    });
    connect(document(), &QTextDocument::contentsChanged, this, [=]() {
        rectLayer->setBounding(boundingRect());
    });
    connect(document(), &QTextDocument::cursorPositionChanged, this, [=](const QTextCursor& cursor) {
        onCursorPositionChange(cursor.position());
    });
    connect(document(), static_cast<void (QTextDocument::*)(int, int, int)>(&QTextDocument::contentsChange),
            this, [=](int position, int charsRemoved, int charsAdded) {
                if(isBrush) {
                    isBrush = false;
                    return;
                }
                QTextCursor textCursor(document());
                if(!commitString) {
                    textCursor.setPosition(position);
                    textCursor.setPosition(position+charsAdded, QTextCursor::KeepAnchor);
                } else {
                    if(charsAdded - charsRemoved == commitLen && position == 0) {
                        textCursor.setPosition(commitPos);
                        textCursor.setPosition(commitPos+commitLen, QTextCursor::KeepAnchor);
                    } else {
                        textCursor.setPosition(position);
                        textCursor.setPosition(position+commitLen, QTextCursor::KeepAnchor);
                    }
                }
                commitString = false;
                textCursor.setCharFormat(currentFormat);
    });
    setFocus();
    FontData data = UIManager::instance()->getFontData();
    currentFormat.setFont(data.font);
    currentFormat.setForeground(data.color);
}


void TextItem::hoverEnterEvent(QGraphicsSceneHoverEvent *event) {
    setFocus();
    if(mEnable) {
        boundShow();
    }
}

void TextItem::hoverLeaveEvent(QGraphicsSceneHoverEvent *event) {
    if(!forceFocus)
        clearFocus();
    boundHide();
}

void TextItem::mousePressEvent(QGraphicsSceneMouseEvent *event) {
    emit requestFocus();
    QGraphicsTextItem::mousePressEvent(event);
    if(nowPos != textCursor().position()) {
        onCursorPositionChange(textCursor().position());
    }
}

void TextItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *event) {
    if(textCursor().selectedText().size() != 0 && isBrush) {
        QTextCursor text_cursor(document());
        text_cursor.setPosition(textCursor().selectionStart());
        text_cursor.setPosition(textCursor().selectionEnd(), QTextCursor::KeepAnchor);
        text_cursor.setCharFormat(brushFormat);
        onCursorPositionChange(textCursor().position());
    }
}

void TextItem::focusInEvent(QFocusEvent *event) {
    QGraphicsTextItem::focusInEvent(event);
}

void TextItem::focusOutEvent(QFocusEvent* event) {
    QGraphicsTextItem::focusOutEvent(event);
}

void TextItem::keyPressEvent(QKeyEvent *event) {
    emit requestFocus();
    QGraphicsTextItem::keyPressEvent(event);
    if(event->key() == Qt::Key_Right || event->key() == Qt::Key_Left) {
        onCursorPositionChange(textCursor().position());
    }
}

void TextItem::inputMethodEvent(QInputMethodEvent *event) {
    commitPos = textCursor().position();
    commitString = true;
    commitLen = event->commitString().length();
    QGraphicsTextItem::inputMethodEvent(event);
}


void TextItem::boundShow() {
    rectLayer->show();
}

void TextItem::boundHide() {
    if(forceFocus) {
        return;
    }
    rectLayer->hide();
    update();
}


void TextItem::onCursorPositionChange(int current) {
    nowPos = current;
    formatChange(textCursor().charFormat().font(), textCursor().charFormat().foreground());
    currentFormat = textCursor().charFormat();
}

void TextItem::getFocus(){
    forceFocus = true;
    update();
}

void TextItem::loseFocus(){
    forceFocus = false;
    boundHide();
    update();
}

void TextItem::setEnable(bool enable){
    this->mEnable = enable;
    if(enable){
        setTextInteractionFlags(Qt::TextEditorInteraction);
    }
    else{
        QTextCursor cursor = textCursor();
        cursor.clearSelection();
        setTextCursor(cursor);
        rectLayer->hide();
        forceFocus = false;
        setTextInteractionFlags(Qt::NoTextInteraction);
    }
    update();
}

void TextItem::setFont(const QFont &font){
    currentFormat.setFont(font);
}

void TextItem::setColor(QColor color){
    currentFormat.setForeground(color);
}

void TextItem::setBrush(bool enable){
    isBrush = enable;
    if(enable) brushFormat = currentFormat;
}

