#ifndef GLOBALKEYTAB_H
#define GLOBALKEYTAB_H
#include <QPushButton>
#include "Manager/IKey_listener.h"
#include "Base/Tab_base.h"
#include <QLabel>
#include <QHBoxLayout>

class GlobalKeyTab : public QWidget, IKey_listener, public Tab_base
{
    Q_OBJECT
public:
    GlobalKeyTab();
    ~GlobalKeyTab();
    GlobalKeyTab(int index, QString name, QString keyName, QWidget* parent);
    void key_end() override;
    void get_key(int key) override;
    void reset() override;
    QString get_name() override;
    int get_begin_index() override;
    int get_default_index() override;
    void restore() override;
private:
    void detectKeyConflict();
    int index;
    int modKey;
    int key;
    int tempKey;
    int originKey;
    int originModKey;
    bool dirty;
    QString name;
    QString keyName;
    QPushButton* modKeyButton;
    QPushButton* keyButton;
    bool listenKey;
    QLabel* icon;
    QPixmap ok;
    QPixmap cancel;
    QHBoxLayout* root;
};

#endif // GLOBALKEYTAB_H
