#include "mainwindow.h"
#include "ui_mainwindow.h"
#include<QtWidgets>
#include<QMessageBox>
#include "new_capture/capture_button_action.h"
#include "window_manager.h"
#include "config.h"


MainWindow::MainWindow(QWidget *parent)
    : Window_base(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    Capture_button_action* new_button = new Capture_button_action(ui->pushButton, this);
    Window_manager::push_window("main", this);
    Window_manager::change_window("main");
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_button_click ()
{
    QMessageBox::information(this, "送餐", "送达");
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    Config::save_to_config();
}

