#include "filechooser.h"
#include <QHBoxLayout>
#include <QToolButton>
#include "Manager/config.h"
#include <QFileDialog>
#include "Helper/imagehelper.h"

FileChooser::FileChooser()
{
}

FileChooser::FileChooser(QString tab_name, QString name, int index, QWidget* parent) : QWidget(parent)
{
    this->tab_name = tab_name;
    this->name = name;
    this->index = index;
    this->f = [=](QString result){
        Config::setConfig(index, currentText);
        Config::updateConfig((Config::setting)index);
    };
    init();
}

FileChooser::FileChooser(QString tab_name, QString name, int index, std::function<void (QString)> const &f, QWidget* parent) : QWidget(parent)
{
    this->tab_name = tab_name;
    this->name = name;
    this->index = index;
    this->f = f;
    init();
}

QString FileChooser::getName()
{
    return tab_name;
}

void FileChooser::reset()
{
    edit->setText(Config::getConfig<QString>((Config::setting)index));
    currentText = Config::getConfig<QString>((Config::setting)index);
}

void FileChooser::init()
{
    QHBoxLayout* base_layout = new QHBoxLayout();
    edit = new QLineEdit(this);
    edit->setText(Config::getConfig<QString>((Config::setting)index));
    connect(edit, &QLineEdit::returnPressed, this, [=](){
        f(edit->text());
    });
    QToolButton* file_button = new QToolButton(this);
    file_button->setIcon(ImageHelper::getIcon("file"));
    connect(file_button, &QToolButton::clicked, this ,[=](){
        QString file_name = QFileDialog::getExistingDirectory(this,
                                                         "保存",
                                                         name,
                                                        QFileDialog::ShowDirsOnly);
        if(file_name != "")
        {
            edit->setText(file_name + "/");
            isChange = true;
            currentText = file_name + "/";
        }
    });
    base_layout->addWidget(edit);
    base_layout->addWidget(file_button);
    setLayout(base_layout);
}

void FileChooser::onSave(){
    if(isChange){
        f(currentText);
    }
}
