#include "debug.h"
#include<QMessageBox>
#include<QDebug>
#include "stackdumper.h"

Debug::Debug()
{

}

void Debug::show_error_message(QString message)
{
    QMessageBox::critical(NULL, "错误", message);
}

void Debug::debug_print_warning(QString message)
{
    qDebug() <<  "\033[32m" << message << "\033[0m";
    TraceStack();
}
