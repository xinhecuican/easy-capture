#ifndef KEY_TAB_H
#define KEY_TAB_H
#include<QPushButton>
#include "Manager/IKeyListener.h"
#include "Base/TabBase.h"

class KeyOption : public QPushButton, IKeyListener, public TabBase {
    Q_OBJECT
public:
    KeyOption();
    KeyOption(int index, QString window_name, QString key_name, QWidget* parent, std::function<void (QString, QString, QList<int>)> const &f);
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
    QList<int> keys;
    QList<int> temp_keys;
    void set_key_string();
    int index;
    bool dirty;
    std::function<void (QString, QString, QList<int>)> f;
};

#endif // KEY_TAB_H
