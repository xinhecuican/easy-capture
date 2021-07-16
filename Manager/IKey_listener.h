#ifndef IKEY_LISTENER_H
#define IKEY_LISTENER_H
class IKey_listener
{
public:
    virtual void get_key(int key)=0;
    virtual void key_end()=0;//没有按键按下了
};

#endif // IKEY_LISTENER_H
