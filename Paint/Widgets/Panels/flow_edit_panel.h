#ifndef FLOW_EDIT_PANEL_H
#define FLOW_EDIT_PANEL_H
#include<QToolBar>
#include<QToolButton>
#include<QColor>
#include<QComboBox>
#include<QPushButton>

class Flow_edit_panel : public QToolBar
{
    Q_OBJECT
public:
    static QFont default_font;
    static QColor default_color;
    static Flow_edit_panel* instance();
    ~Flow_edit_panel();
    QFont get_font();
    QColor get_color();
    void set_format(QFont font, QColor color);
signals:
    void font_change();
    void text_brush();//格式刷
private:
    Flow_edit_panel();

    static Flow_edit_panel* _instance;
    QToolButton* color_selector_button;
    QComboBox *m_ComboBox;
    QToolButton* bold_button;
    QToolButton* italic_button;
    QToolButton* underline_button;
    QColor color;
    QFont font;
    QComboBox* width_button;
    QList<QString> get_font_text();
};

#endif // FLOW_EDIT_PANEL_H
