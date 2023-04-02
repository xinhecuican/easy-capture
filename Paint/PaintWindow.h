#ifndef PAINT_WINDOW_H
#define PAINT_WINDOW_H

#include <QMainWindow>
#include<QWidget>
#include "Base/WindowBase.h"
#include "Paint/Widgets/PaintArea.h"
#include<QScrollArea>
#include<QDockWidget>
#include<QToolButton>
#include "Paint/Widgets/Panels/paint_setting_panel.h"
#include<QMenuBar>
#include<QGraphicsView>
#include <QTimer>

//namespace Ui {
//class PaintWindow;
//}

class PaintWindow : public WindowBase {
    Q_OBJECT

public:
    friend class Paint_setting_panel;
    Q_INVOKABLE explicit PaintWindow(QWidget *parent = nullptr);
    ~PaintWindow();
    void set_menubar();
    void set_toolbar();
    void loadKeyEvent(QString name) override;
    void reset();
    void setPic(QPixmap pix, QRect rect)override;
    void onWindowCancel() override;
    void closeEvent(QCloseEvent* event)override;
    void onPaintWindowClose() override;
    void onWindowClose() override;
public slots:
//    void append_layer();
//    void remove_layer(int index);
//    void layer_rename(int index, QString after_name);
//    void change_layer_position(int before_index, int after_index);
//    QStringList get_layer_name();
    void showSettingPanel();
private:
//    Ui::PaintWindow *ui;
    QWidget* centralWidget;
    QToolBar* toolbar;
    PaintArea* area;
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
