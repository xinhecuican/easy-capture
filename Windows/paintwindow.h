#ifndef PAINT_WINDOW_H
#define PAINT_WINDOW_H

#include <QMainWindow>
#include<QWidget>
#include "../Base/WindowBase.h"
#include "../GraphicsScene/paintarea.h"
#include<QScrollArea>
#include<QDockWidget>
#include<QToolButton>
#include<QMenuBar>
#include<QGraphicsView>
#include <QTimer>
#include "../GraphicsScene/Widgets/defaulttoolbar.h"

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
    void loadKeyEvent(QString name) override;
    void reset();
    void onWindowCancel() override;
    void onWindowSelect() override;
    void closeEvent(QCloseEvent* event)override;
    void onPaintWindowClose() override;
    void onWindowClose() override;
    /**
     * @brief receiveData
     * @param data1 iamge: 接收的图像
     * @param data2 rect: 图像的大小
     */
    void receiveData(QVariant data1 =QVariant(), QVariant data2 =QVariant()) override;

private:
    //    Ui::PaintWindow *ui;
    QWidget* centralWidget;
    DefaultToolbar* toolbar;
    PaintArea* area;
    QGraphicsView* paint_panel;
    QHBoxLayout* layout;
    QMenuBar* menuBar;
};

#endif // PAINT_WINDOW_H
