#include "debug.h"
#include<QMessageBox>

Debug::Debug()
{

}

void Debug::show_error_message(QString message)
{
    QMessageBox::critical(NULL, "错误", message);
}
