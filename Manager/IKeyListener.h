#ifndef IKEY_LISTENER_H
#define IKEY_LISTENER_H
class IKeyListener {
public:
    virtual void getKey(int key)=0;
    virtual void keyEnd()=0;//没有按键按下了
};

#endif // IKEY_LISTENER_H
