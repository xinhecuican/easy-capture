#include "capture_button_action.h"
#include "window_manager.h"
#include<QDebug>

Capture_button_action::Capture_button_action(QAbstractButton* source, QWidget* parent)
{
    this->source_button = source;
    this->parent = parent;
    connect(source_button, SIGNAL(clicked()), this, SLOT(on_button_click()));
    m_window = NULL;
}

void Capture_button_action::on_button_click()
{
    if(m_window == NULL )
    {
        m_window = new Capture_window(parent);
    }
    Window_manager::change_window("capture");
}

Capture_button_action::~Capture_button_action()
{
    if(m_window != NULL)
    {
        delete m_window;
    }
}
