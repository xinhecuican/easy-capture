#include "text_edit.h"
#include<QDebug>
#include "Paint/Widgets/Panels/flow_edit_panel.h"
#include<QPainter>
#include<QScrollBar>
#include<QTextBlock>

Text_edit::Text_edit(QWidget* parent):QTextEdit(parent)
{
    document()->adjustSize();
    QTextCharFormat format;
    format.setFont(Flow_edit_panel::default_font);
    setCurrentCharFormat(format);
    textCursor().setCharFormat(format);
    setFont(Flow_edit_panel::default_font);
    Flow_edit_panel::instance()->set_format(Flow_edit_panel::instance()->default_font, QColor(0, 0, 0));
    can_brush = false;
    new_format_enable = false;
    now_height = 23;
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    connect(Flow_edit_panel::instance(), &Flow_edit_panel::font_change, this, [=](){
        QTextCharFormat format;
        format.setForeground(Flow_edit_panel::instance()->get_color());
        format.setFont(Flow_edit_panel::instance()->get_font());
        setCurrentCharFormat(format);
        new_format_enable = true;
        this->setFocus();
    });
    connect(this, &QTextEdit::cursorPositionChanged, this, [=](){
        if(!new_format_enable)
        {
            Flow_edit_panel::instance()->set_format(textCursor().charFormat().font(),
                                                textCursor().charFormat().foreground().color());
        }
    });
    connect(this, &QTextEdit::copyAvailable, this, [=](bool yes){
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
    connect(this, &QTextEdit::selectionChanged, this, [=](){
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
    connect(document(), &QTextDocument::contentsChanged, this, [=](){
        int width = document()->size().width()+1;
        int height = document()->size().height()+1;
        resize(width, height);
        emit sizeChange(width, height);
        update();
    });
}

void Text_edit::inputMethodEvent(QInputMethodEvent* event)
{
    QTextCursor cursor = textCursor();
    if(cursor.selectionEnd() - cursor.selectionStart() != 0)
    {
        textCursor().removeSelectedText();
    }
    new_format_enable = false;
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
        int pre_height = cursor.blockFormat().lineHeight();
        int block_count = document()->lineCount();
        cursor.deletePreviousChar();
        if(block_count != document()->lineCount())
        {
            now_height -= pre_height;
            resize(document()->size().width(), now_height);
        }
        return;
    }
    new_format_enable = false;
    insert_text(text);
}

void Text_edit::insert_text(QString str)
{
    textCursor().insertText(str);
    document()->adjustSize();
}
