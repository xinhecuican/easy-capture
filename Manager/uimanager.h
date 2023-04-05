#ifndef UIMANAGER_H
#define UIMANAGER_H
#include <QString>

class UIManager {
public:
    UIManager();
    ~UIManager();
    static UIManager* instance() {
        if(_instance == nullptr) {
            _instance = new UIManager();
        }
        return _instance;
    }

    QString load(QString name);
    void save(QString path);

private:
    static UIManager* _instance;
};

#endif // UIMANAGER_H
