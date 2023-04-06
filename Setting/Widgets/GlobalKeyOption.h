#ifndef GLOBALKEYTAB_H
#define GLOBALKEYTAB_H
#include <QPushButton>
#include "Manager/IKeyListener.h"
#include "Base/TabBase.h"
#include <QLabel>
#include <QHBoxLayout>

class GlobalKeyOption : public QWidget, IKeyListener, public TabBase {
    Q_OBJECT
public:
    GlobalKeyOption();
    ~GlobalKeyOption();
    GlobalKeyOption(int index, QString name, QString keyName, QWidget* parent, std::function<void (QString, int, int)> const &f);
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
    std::function<void (QString, int, int)> f;
};

#endif // GLOBALKEYTAB_H
