#ifndef DEBUG_H
#define DEBUG_H
#include<QString>

class Debug
{
public:
    Debug();
    static void show_error_message(QString message);
    static void debug_print_warning(QString message);
};

#endif // DEBUG_H
