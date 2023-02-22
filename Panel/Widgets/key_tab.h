#ifndef KEY_TAB_H
#define KEY_TAB_H
#include<QPushButton>
#include "Manager/IKey_listener.h"
#include "Base/Tab_base.h"

class Key_tab : public QPushButton, IKey_listener, public Tab_base
{
    Q_OBJECT
public:
    Key_tab();
    Key_tab(int index, QString window_name, QString key_name, QWidget* parent);
    void key_end() override;
    void get_key(int key) override;
    void reset() override;
    void restore() override;
    QString get_name() override;
    int get_begin_index() override;
    int get_default_index() override;

signals:
    void key_conflict(QList<QString> key_names);
private:
    QString window_name;
    QString key_name;
    QList<int> originKeys;
    QList<int> keys;
    QList<int> temp_keys;
    void set_key_string();
    int index;
    bool dirty;
};

#endif // KEY_TAB_H
