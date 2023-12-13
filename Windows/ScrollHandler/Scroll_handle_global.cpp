#include "Scroll_handler_global.h"

Scroll_handler_global::~Scroll_handler_global() {
    _instance = NULL;
}

std::atomic<Scroll_handler_global*> Scroll_handler_global::_instance = {nullptr};
std::mutex Scroll_handler_global::mutex;
