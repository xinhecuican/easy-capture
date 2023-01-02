#ifndef PAINT_SETTING_PANEL_H
#define PAINT_SETTING_PANEL_H
#include<QScrollArea>
#include<QVBoxLayout>
#include<QDialog>
#include<QDockWidget>
#include<QComboBox>
#include "Paint/Widgets/Layer_list/list_widget.h"
#include "Paint/Data/Common.h"

class Paint_setting_panel : public QDockWidget
{
    Q_OBJECT
public:
    friend class Paint_window;
    static Paint_setting_panel* instance(QWidget* parent=nullptr);
    Paint_setting_panel();
    Paint_setting_panel(QWidget* parent = nullptr);
    ~Paint_setting_panel();
    void init();
    void closeEvent(QCloseEvent *event) override;
    void set_style();
signals:
    void disable_color_change(int index, QColor color=QColor());
    void saveDisableColorChange(int index, QColor color=QColor());
    void paint_shape(SHAPE_TYPE type);
    void layer_rename(int index, QString after_name);
    void remove_layer(int index);
    void append_layer();
    void change_layer_position(int before_index, int after_index);
    void requestLayersName();
public slots:
    void receiveLayerName(QString name);
    void receiveLayersName(QStringList name);
private:
    static Paint_setting_panel* _instance;
    QWidget* parent;
    QWidget* base;
    QScrollArea* area;
    QVBoxLayout* layout;
    QPushButton* color_button;
    QPushButton* back_button;
    QComboBox* width_button;
    List_widget* list_widget;
    void init_pen_setting();
    void init_layer_setting();
    void init_disable_color_setting();
    void init_shape_setting();
};

#endif // PAINT_SETTING_PANEL_H
