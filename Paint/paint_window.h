#ifndef PAINT_WINDOW_H
#define PAINT_WINDOW_H

#include <QMainWindow>
#include<QWidget>
#include "Base/Window_base.h"
#include "Paint/Widgets/Paint_area.h"
#include<QScrollArea>
#include<QDockWidget>
#include<QToolButton>
#include "Paint/Widgets/Panels/paint_setting_panel.h"
#include<QMenuBar>
#include<QGraphicsView>
#include <QTimer>

namespace Ui {
class Paint_window;
}

class Paint_window : public Window_base
{
    Q_OBJECT

public:
    friend class Paint_setting_panel;
    Q_INVOKABLE explicit Paint_window(QWidget *parent = nullptr);
    ~Paint_window();
    void set_menubar();
    void set_toolbar();
    void load_key_event(QString name) override;
    void reset();
    void set_pic(QPixmap pix, QRect rect)override;
    void on_window_cancal() override;
    void closeEvent(QCloseEvent* event)override;
    void on_paint_panel_close() override;
    void on_window_close() override;
public slots:
//    void append_layer();
//    void remove_layer(int index);
//    void layer_rename(int index, QString after_name);
//    void change_layer_position(int before_index, int after_index);
//    QStringList get_layer_name();
    void showSettingPanel();
private:
    Ui::Paint_window *ui;
    Paint_area* area;
    QGraphicsView* paint_panel;
    QHBoxLayout* layout;
    QToolButton* pencil_button;
    QMenuBar* menu_bar;
    QToolButton* cursor_button;
    QCursor pen_cursor;
    QButtonGroup* paint_button_group;
    void initSettingPanel();
};

#endif // PAINT_WINDOW_H
