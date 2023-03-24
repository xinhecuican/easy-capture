#ifndef KEY_TAB_H
#define KEY_TAB_H
#include<QPushButton>
#include "Manager/IKeyListener.h"
#include "Base/TabBase.h"

class Key_tab : public QPushButton, IKeyListener, public TabBase
{
    Q_OBJECT
public:
    Key_tab();
    Key_tab(int index, QString window_name, QString key_name, QWidget* parent);
    void keyEnd() override;
    void getKey(int key) override;
    void reset() override;
    void restore() override;
    QString getName() override;
    int getBeginIndex() override;
    int getDefaultIndex() override;

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
