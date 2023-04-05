#ifndef FILECHOOSER_H
#define FILECHOOSER_H
#include <QWidget>
#include <QLabel>
#include <QLineEdit>
#include "Base/TabBase.h"

class FileChooser : public QWidget, public TabBase {
    Q_OBJECT
public:
    FileChooser();
    FileChooser(QString tab_name, QString name, int index, std::function<void (QString)> const &f, QWidget* parent);
    QString getName() override;
    void reset() override;
    int getBeginIndex() override;
    int getDefaultIndex() override;
private:
    void init();
    QString tab_name;
    QString name;
    int index;
    QWidget* parent;
    QLineEdit* edit;
    std::function<void (QString)> f;
};

#endif // FILECHOOSER_H
