#include "mainwindow.h"

#include <QApplication>
#include<QTextCodec>
#include "config.h"

int main(int argc, char *argv[])
{
    Config::load_config();
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();
}
