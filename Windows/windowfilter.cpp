#include "windowfilter.h"

HWND Window_fliter::m_hWndTarget = NULL;
QVector<CSCWinInfo*> Window_fliter::m_arSnapshot = QVector<CSCWinInfo*>();
Window_fliter* Window_fliter::_instance = NULL;
HWND Window_fliter::scroll_hwnd = NULL;
