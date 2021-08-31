#include "Scroll_handler_global.h"

Scroll_handler_global::~Scroll_handler_global()
{
    _instance = NULL;
}

Scroll_handler_global* Scroll_handler_global::_instance = NULL;
