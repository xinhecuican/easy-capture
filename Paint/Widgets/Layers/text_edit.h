#ifndef TEXT_EDIT_H
#define TEXT_EDIT_H
#include<QPlainTextEdit>
#include "Paint/Data/Common.h"

class Text_edit : public QTextEdit
{
    Q_OBJECT
public:
    Text_edit(QWidget* parent=nullptr);
    void inputMethodEvent(QInputMethodEvent* event) override;
    void keyPressEvent(QKeyEvent *e) override;
signals:
    void text_change(int pos, int remove, int add, QString text);
    void sizeChange(int width, int height);
private:
    bool is_text_brush;
    bool can_brush;
    bool new_format_enable;
    int now_height;
    int text_len;
    QTextCharFormat brush_format;
    font_data now_data;
    void insert_text(QString str);
};

#endif // TEXT_EDIT_H
