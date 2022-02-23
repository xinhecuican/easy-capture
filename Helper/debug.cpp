#include "debug.h"
#include<QMessageBox>
#include<QDebug>
#include<QMainWindow>
#include<QLabel>
#include<QScrollArea>

void Debug::show_error_message(QString message)
{
    QMessageBox::critical(NULL, "错误", message);
}

void Debug::debug_print_warning(QString message)
{
    qDebug() <<  "\033[32m" << message << "\033[0m";
}

void Debug::showImage(QImage image)
{
    QMainWindow mw;

    QLabel *label=new QLabel();
    label->setPixmap(QPixmap::fromImage(image));

    QScrollArea *sa=new QScrollArea(&mw);
    sa->setWidget(label);
    sa->resize(400,400);
    mw.show();
}
