#include "text_edit.h"
#include<QDebug>
#include "Paint/Widgets/Panels/flow_edit_panel.h"
#include<QPainter>

Text_edit::Text_edit(QWidget* parent):QPlainTextEdit(parent)
{
    QTextCharFormat format;
    format.setForeground(Flow_edit_panel::instance()->get_color());
    format.setFont(Flow_edit_panel::instance()->get_font());
    setCurrentCharFormat(format);
    can_brush = false;
    connect(Flow_edit_panel::instance(), &Flow_edit_panel::font_change, this, [=](){
        QTextCharFormat format;
        format.setForeground(Flow_edit_panel::instance()->get_color());
        format.setFont(Flow_edit_panel::instance()->get_font());
        setCurrentCharFormat(format);
        this->setFocus();
    });
    connect(this, &QPlainTextEdit::cursorPositionChanged, this, [=](){
        Flow_edit_panel::instance()->set_format(textCursor().charFormat().font(),
                                                textCursor().charFormat().foreground().color());
    });
    connect(this, &QPlainTextEdit::copyAvailable, this, [=](bool yes){
        if(is_text_brush && yes)
        {
            is_text_brush = false;
            can_brush = true;
        }
        else if(!yes)
        {
            can_brush = false;
        }
    });
    connect(this, &QPlainTextEdit::selectionChanged, this, [=](){
        int len = textCursor().selectedText().length();
        if(can_brush && len > text_len)
        {
            text_len = len;
            textCursor().setCharFormat(brush_format);
        }
    });
    connect(Flow_edit_panel::instance(), &Flow_edit_panel::text_brush, this, [=](){
        is_text_brush = true;
        text_len = 0;
        brush_format = currentCharFormat();
    });
}

void Text_edit::inputMethodEvent(QInputMethodEvent* event)
{
    QTextCursor cursor = textCursor();
    if(cursor.selectionEnd() - cursor.selectionStart() != 0)
    {
        textCursor().removeSelectedText();
    }
    insert_text(event->commitString());
}

void Text_edit::keyPressEvent(QKeyEvent *e)
{
    if(e->text() == "")
    {
        return;
    }
    QString text = e->text();
    QTextCursor cursor = textCursor();
    if(e->text() == "\t")
    {
        text = "    ";
    }
    else if(e->text() == "\b")//backspace
    {
        cursor.deletePreviousChar();
        return;
    }
    insert_text(text);
}

void Text_edit::insert_text(QString str)
{
    QFontMetrics metrics = fontMetrics();
    QString ans_str = str;
    int insert_size = 0;
    for(int i=0; i<str.size(); i++)
    {
        int width = metrics.horizontalAdvance(str[i]);
        if(cursorRect().right() + width > this->rect().right() && str[i] != '\xa')
        {
            ans_str.insert(i+insert_size, '\xa');
        }
    }
    textCursor().insertText(ans_str);
}
