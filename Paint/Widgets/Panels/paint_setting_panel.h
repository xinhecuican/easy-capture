#ifndef PAINT_SETTING_PANEL_H
#define PAINT_SETTING_PANEL_H
#include<QScrollArea>
#include<QVBoxLayout>
#include<QDialog>
#include<QDockWidget>
#include<QComboBox>
#include "Paint/Widgets/Layer_list/list_widget.h"
#include "Paint/Widgets/IControl_layer_change.h"
#include "Paint/Data/Common.h"

class Paint_setting_panel : public QDockWidget
{
    Q_OBJECT
public:

    Paint_setting_panel();
    Paint_setting_panel(IControl_layer_change* layer_control, QWidget* parent = nullptr);
    ~Paint_setting_panel();
    void init();
    void closeEvent(QCloseEvent *event) override;
    void set_style();
signals:
    void disable_color_change(int index, QColor color=QColor());
    void paint_shape(shape_type type);
private:
    IControl_layer_change* layer_control;
    QWidget* parent;
    QWidget* base;
    QScrollArea* area;
    QVBoxLayout* layout;
    QPushButton* color_button;
    QPushButton* back_button;
    QComboBox* width_button;
    void init_pen_setting();
    void init_layer_setting();
    void init_disable_color_setting();
    void init_shape_setting();
};

#endif // PAINT_SETTING_PANEL_H
