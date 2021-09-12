#ifndef PAINT_WINDOW_H
#define PAINT_WINDOW_H

#include <QMainWindow>
#include<QWidget>
#include "Base/Window_base.h"
#include "Paint/Widgets/Paint_area.h"
#include "new_capture/capture_button_action.h"
#include<QScrollArea>
#include<QDockWidget>
#include<QToolButton>
#include "Paint/Widgets/Panels/paint_setting_panel.h"
#include "Widgets/IControl_layer_change.h"

namespace Ui {
class Paint_window;
}

class Paint_window : public Window_base, IControl_layer_change
{
    Q_OBJECT

public:
    Q_INVOKABLE explicit Paint_window(QWidget *parent = nullptr);
    ~Paint_window();
    void set_menubar();
    void set_toolbar();
    void load_key_event(QString name) override;
    void reset();
    void changeEvent(QEvent* event)override;
    void set_pic(QPixmap pix, QRect rect)override;
    void on_window_cancal() override;
    void closeEvent(QCloseEvent* event)override;
    void on_paint_panel_close() override;
    void on_window_close() override;
    QString append_layer() override;
    void remove_layer(int index) override;
    void layer_rename(int index, QString after_name) override;
    void change_layer_position(int before_index, int after_index) override;
    QStringList get_layer_name() override;

private:
    Ui::Paint_window *ui;
    Paint_area* area;
    Capture_button_action* new_button_action;
    QScrollArea* paint_panel;
    QHBoxLayout* layout;
    QToolButton* pencil_button;
    Paint_setting_panel* paint_setting_panel;
};

#endif // PAINT_WINDOW_H
