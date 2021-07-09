#include "list_item.h"
#include<QBoxLayout>
#include<QPushButton>
#include<QToolButton>


List_item::List_item()
{

}

List_item::List_item(QWidget* parent, QString text, bool useable) :QWidget(parent)
{
    QHBoxLayout* layout = new QHBoxLayout(this);
    QVBoxLayout* move_button_layout = new QVBoxLayout(this);
    QToolButton* up_button = new QToolButton(this);
    up_button->setIcon(QIcon(":/image/up.png"));
    up_button->setIconSize(QSize(20, 20));
    QToolButton* down_button = new QToolButton(this);
    down_button->setIcon(QIcon(":/image/down.png"));
    down_button->setIconSize(QSize(20, 20));
    connect(up_button, &QPushButton::clicked, this, [=](){
        emit button_click(UP);
    });
    connect(down_button, &QPushButton::clicked, this, [=](){
        emit button_click(DOWN);
    });
    move_button_layout->addWidget(up_button);
    move_button_layout->addWidget(down_button);
    layout->addLayout(move_button_layout);

    name = new QLineEdit(this);
    name->setText(text);
    name->setAlignment(Qt::AlignLeft);
    connect(name, &QLineEdit::returnPressed, this, [=](){
        emit text_change(name->text());
    });
    QToolButton* close_button = new QToolButton(this);
    close_button->setIcon(QIcon(":/image/cancel.svg"));
    connect(close_button, &QPushButton::clicked, this, [=](){
        emit button_click(CLOSE);
    });
    if(!useable)
    {
        name->setReadOnly(true);
        close_button->setEnabled(false);
    }
    layout->addWidget(name);
    layout->addWidget(close_button);
    setLayout(layout);
}

QString List_item::get_name()
{
    return name->text();
}

void List_item::set_name(QString name)
{
    this->name->setText(name);
    update();
}
