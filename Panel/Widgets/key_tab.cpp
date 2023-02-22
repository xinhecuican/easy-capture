#include "key_tab.h"
#include "Manager/key_manager.h"

Key_tab::Key_tab()
{

}

Key_tab::Key_tab(int index, QString window_name, QString key_name, QWidget* parent) : QPushButton(parent)
{
    this->index = index;
    this->window_name = window_name;
    this->key_name = key_name;
    keys = Key_manager::get_keys(window_name, key_name);
    set_key_string();
    setCheckable(true);
    connect(this, &QPushButton::clicked, this, [=](){
        setText("");
        temp_keys = QList<int>();
        Key_manager::add_key_listener(this);
    });
    originKeys = keys;
    dirty = false;
}

void Key_tab::get_key(int key)
{
    temp_keys.append(key);
    if(text() == "")
    {
        setText(text() + Key_manager::key_type[key]);
    }
    else
    {
        setText(text() + "+" + Key_manager::key_type[key]);
    }
}

void Key_tab::key_end()
{
    Key_manager::remove_key_listener(this);
    QList<QString> key_names = Key_manager::detect_key_conflict(window_name, key_name, temp_keys);
    if(key_names.size() != 0)
    {
        for(int i=0; i<key_names.size(); i++)
        {
            key_names[i] = window_name + key_names[i];
        }

        emit key_conflict(key_names);
    }
    else if(temp_keys.size() != 0)
    {
        setChecked(false);
        Key_manager::set_keys(window_name, key_name, temp_keys);
        keys = temp_keys;
        dirty = true;
    }
    else
    {
        set_key_string();
        setChecked(false);
    }
}

void Key_tab::set_key_string()
{
    setText(Key_manager::key_type[keys[0]]);
    for(int i=1; i<keys.size(); i++)
    {
        setText(text() + "+" + Key_manager::key_type[keys[i]]);
    }
}

void Key_tab::reset()
{

}

int Key_tab::get_begin_index()
{
    return 0;
}

int Key_tab::get_default_index()
{
    return index;
}

QString Key_tab::get_name()
{
    return window_name + key_name;
}

void Key_tab::restore(){
    if(dirty){
        Key_manager::set_keys(window_name, key_name, originKeys);
    }
}
