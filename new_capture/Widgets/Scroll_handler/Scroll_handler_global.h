#ifndef SCROLL_HANDLER_GLOBAL_H
#define SCROLL_HANDLER_GLOBAL_H
#include<iostream>
class Scroll_handler_global
{
public:
    Scroll_handler_global()
    {
        middle_width = 0;
    }

    ~Scroll_handler_global();

    static Scroll_handler_global* instance()
    {
        if(_instance == NULL)
        {
            _instance = new Scroll_handler_global();
        }
        return _instance;
    }

    void cal_middle_width(int new_width)
    {
        if(middle_width == 0)
        {
            middle_width = new_width;
            return;
        }
        middle_width = (float)middle_width * 0.9 + (float)new_width * 0.1;
    }
    int middle_width;
private:
    static Scroll_handler_global* _instance;
};

#endif // SCROLL_HANDLER_GLOBAL_H
