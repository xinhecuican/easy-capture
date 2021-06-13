#include "capture_button_action.h"
#include "Manager/window_manager.h"
#include<QDebug>

Capture_button_action::Capture_button_action(QAbstractButton* source, QWidget* parent)
{
    this->source_button = source;
    this->parent = parent;
    connect(source_button, SIGNAL(clicked()), this, SLOT(on_button_click()));
}

void Capture_button_action::on_button_click()
{
    Window_manager::change_window("Capture_window");
}

Capture_button_action::~Capture_button_action()
{
}
