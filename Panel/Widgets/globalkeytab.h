#ifndef GLOBALKEYTAB_H
#define GLOBALKEYTAB_H
#include <QPushButton>
#include "Manager/IKeyListener.h"
#include "Base/TabBase.h"
#include <QLabel>
#include <QHBoxLayout>

class GlobalKeyTab : public QWidget, IKeyListener, public TabBase
{
    Q_OBJECT
public:
    GlobalKeyTab();
    ~GlobalKeyTab();
    GlobalKeyTab(int index, QString name, QString keyName, QWidget* parent);
    void keyEnd() override;
    void getKey(int key) override;
    void reset() override;
    QString getName() override;
    int getBeginIndex() override;
    int getDefaultIndex() override;
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
