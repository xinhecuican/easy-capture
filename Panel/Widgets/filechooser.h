#ifndef FILECHOOSER_H
#define FILECHOOSER_H
#include <QWidget>
#include <QLabel>
#include <QLineEdit>
#include "Base/Tab_base.h"

class FileChooser : public QWidget, public Tab_base
{
public:
    FileChooser();
    FileChooser(QString tab_name, QString name, int index, std::function<void (QString)> const &f, QWidget* parent);
    QString get_name() override;
    void reset() override;
    int get_begin_index() override;
    int get_default_index() override;
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
