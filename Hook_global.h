#ifndef HOOK_GLOBAL_H
#define HOOK_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(HOOK_LIBRARY)
#  define HOOK_EXPORT Q_DECL_EXPORT
#else
#  define HOOK_EXPORT Q_DECL_IMPORT
#endif

#endif // LIB_GLOBAL_H
