#ifndef PAINT_SETTING_PANEL_H
#define PAINT_SETTING_PANEL_H
#include<QScrollArea>
#include<QVBoxLayout>
#include<QDialog>
#include<QDockWidget>
#include<QComboBox>
#include "Layer_list/list_widget.h"
#include "IControl_layer_change.h"

class Paint_setting_panel : public QDockWidget
{
public:
    Paint_setting_panel();
    Paint_setting_panel(IControl_layer_change* layer_control, QWidget* parent = nullptr);
    ~Paint_setting_panel();
    void init();
    void closeEvent(QCloseEvent *event) override;
    void set_style();
private:
    IControl_layer_change* layer_control;
    QWidget* parent;
    QWidget* base;
    QScrollArea* area;
    QVBoxLayout* layout;
    QPushButton* color_button;
    QComboBox* width_button;
    void init_pen_setting();
    void init_layer_setting();
};

#endif // PAINT_SETTING_PANEL_H
