#ifndef FLOW_EDIT_PANEL_H
#define FLOW_EDIT_PANEL_H
#include<QToolBar>
#include<QToolButton>
#include<QColor>
#include<QComboBox>
#include<QPushButton>

class Flow_edit_panel : public QToolBar
{
public:
    static Flow_edit_panel* instance();
    ~Flow_edit_panel();
private:
    Flow_edit_panel();

    static Flow_edit_panel* _instance;
    QToolButton* color_selector_button;
    QColor color;
    QFont font;
    QComboBox* width_button;
    QList<QString> get_font_text();
};

#endif // FLOW_EDIT_PANEL_H
