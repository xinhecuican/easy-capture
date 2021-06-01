#include "mainwindow.h"
#include "ui_mainwindow.h"
#include<QtWidgets>
#include<QMessageBox>
#include "button_action/headers/base_button.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    button_action* test_button = new button_action(ui->pushButton, this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_button_click ()
{
    QMessageBox::information(this, "送餐", "送达");
}

