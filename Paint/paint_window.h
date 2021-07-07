#ifndef PAINT_WINDOW_H
#define PAINT_WINDOW_H

#include <QMainWindow>
#include<QWidget>
#include "Base/Window_base.h"
#include "Widgets/paint_area.h"
#include "new_capture/capture_button_action.h"
#include<QScrollArea>
#include<QDockWidget>

namespace Ui {
class Paint_window;
}

class Paint_window : public Window_base
{
    Q_OBJECT

public:
    Q_INVOKABLE explicit Paint_window(QWidget *parent = nullptr);
    ~Paint_window();
    void set_menubar();
    void set_toolbar();
    void set_pic(QPixmap pix, QRect rect)override;
    void closeEvent(QCloseEvent* event)override;

private:
    Ui::Paint_window *ui;
    Paint_area* area;
    Capture_button_action* new_button_action;
    QScrollArea* paint_panel;
    QHBoxLayout* layout;
};

#endif // PAINT_WINDOW_H
