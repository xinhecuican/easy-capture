#include "button_action/headers/base_button.h"
#include<QMessageBox>
#include<iostream>
using namespace std;

button_action::button_action(QPushButton* source, QWidget* parent) : QWidget(parent)
{
    this->source_button = source;
    this->parent = parent;
    connect(source_button, SIGNAL(clicked()), this, SLOT(on_button_click ()));
}

void button_action::on_button_click()
{
    cout << "1" << endl;
}


