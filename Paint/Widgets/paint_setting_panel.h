#ifndef PAINT_SETTING_PANEL_H
#define PAINT_SETTING_PANEL_H
#include<QScrollArea>
#include<QVBoxLayout>
#include<QDialog>
#include<QDockWidget>

class Paint_setting_panel : public QDockWidget
{
public:
    Paint_setting_panel();
    Paint_setting_panel(QWidget* parent = nullptr);
    void init();
private:
    QWidget* parent;
    QWidget* base;
    QScrollArea* area;
    QVBoxLayout* layout;
};

#endif // PAINT_SETTING_PANEL_H
