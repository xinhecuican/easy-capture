#ifndef ICONTROL_LAYER_CHANGE_H
#define ICONTROL_LAYER_CHANGE_H
#include<QString>
class IControl_layer_change
{
public:
    virtual void layer_rename(int index, QString after_name)=0;
    virtual void remove_layer(int index)=0;
    virtual QString append_layer()=0;
    virtual void change_layer_position(int before_index, int after_index)=0;
    virtual QStringList get_layer_name()=0;
};

#endif // ICONTROL_LAYER_CHANGE_H
