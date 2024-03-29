#ifndef TAB_WIDGET_H
#define TAB_WIDGET_H

#include <QWidget>
#include<QVector>
#include<functional>
#include<QRadioButton>
#include<QVBoxLayout>
#include<QScrollArea>
#include "BoolOption.h"
#include "ComboOption.h"
using namespace std;

class TabWidget : public QScrollArea {
    Q_OBJECT
public:
    TabWidget();
    TabWidget(QWidget* parent);
    TabWidget(QString name, QWidget* parent);
    ~TabWidget();
    void add_bool_option(QString tab_name, QString name, int index, std::function<void (bool)> const  &f);
    void add_combo_option(QString tab_name, QString text, QVector<QString> name,
                          int index, std::function<void (int)> const &f, bool isString=false);
    void add_key_option(int index, QString tab_name, QString window_name, QString key_name, std::function<void (QString, QString, QList<int>)> const &f);
    void addGlobalKeyOption(int index, QString tabName, QString keyName, std::function<void (QString, int, int)> const &f);
    void add_num_option(QString tab_name, int index, QString name, int min, int max,
                        std::function<void(int)> const &f);
    void add_file_option(QString tab_name, QString name, int index, std::function<void(QString)> const &f);
    void addBoolOption(QString tab_name, QString name, int index);
    void addComboOption(QString tab_name, QString text, QVector<QString> name, int index, bool isString=false);
    void addKeyOption(int index, QString tab_name, QString window_name, QString key_name);
    void addGlobalKeyOption(int index, QString tabName, QString keyName);
    void addNumOption(QString tab_name, int index, QString name, int min, int max);
    void addFileOption(QString tab_name, QString name, int index);
    void add_spacer(QString text = QString());
    void addText(QString text, QString objectName);
    void add_layout(QLayout* layout);
    void reset();
    void restore();
    bool isChange();
    QString getName();
    void onSave();
private:
    QVector<TabBase*> widgets;
    QString widget_name;
    QVBoxLayout* layout;
    QWidget* base;
    void init();
};

#endif // TAB_WIDGET_H
