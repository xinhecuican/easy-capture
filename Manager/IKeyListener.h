#ifndef IKEY_LISTENER_H
#define IKEY_LISTENER_H
class IKeyListener {
public:
    virtual void getKey(int key)=0;

    /**
     * @brief keyEnd 当有按键释放时触发
     */
    virtual void keyEnd()=0;
};

#endif // IKEY_LISTENER_H
