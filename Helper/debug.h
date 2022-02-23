#ifndef DEBUG_H
#define DEBUG_H
#include<QString>
#include<QImage>

class Debug
{
public:
    static void show_error_message(QString message);
    static void debug_print_warning(QString message);
    static void showImage(QImage image);
};

#endif // DEBUG_H
