#ifndef FLOW_EDIT_PANEL_H
#define FLOW_EDIT_PANEL_H
#include<QToolBar>
#include<QToolButton>
#include<QColor>
#include<QComboBox>
#include<QPushButton>
#include "Paint/Data/fontdata.h"

/**
 * @brief 绘制文字的工具栏
 */
class Flow_edit_panel : public QToolBar {
    Q_OBJECT
public:
    static Flow_edit_panel* instance();
    ~Flow_edit_panel();
    QFont get_font();
    QColor get_color();
    void set_format(QFont font, QColor color);
    void reset();
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
    QComboBox* width_button;
    QList<QString> get_font_text();
    FontData fontData;
};

#endif // FLOW_EDIT_PANEL_H
