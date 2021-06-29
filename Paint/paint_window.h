#ifndef PAINT_WINDOW_H
#define PAINT_WINDOW_H

#include <QMainWindow>
#include "Base/Window_base.h"

namespace Ui {
class Paint_window;
}

class Paint_window : public Window_base
{
    Q_OBJECT

public:
    explicit Paint_window(QWidget *parent = nullptr);
    ~Paint_window();

private:
    Ui::Paint_window *ui;
};

#endif // PAINT_WINDOW_H
