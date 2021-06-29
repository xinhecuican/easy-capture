#ifndef TAB_BASE_H
#define TAB_BASE_H
#include<QString>
class Tab_base
{
public:
    virtual QString get_name()=0;
    virtual int get_begin_index()=0;
    virtual int get_default_index()=0;
    virtual void reset()=0;
};

#endif // TAB_BASE_H
