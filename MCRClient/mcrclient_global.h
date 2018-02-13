#ifndef MCRCLIENT_GLOBAL_H
#define MCRCLIENT_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(MCRCLIENT_LIBRARY)
#  define MCRCLIENTSHARED_EXPORT Q_DECL_EXPORT
#else
#  define MCRCLIENTSHARED_EXPORT Q_DECL_IMPORT
#endif

#endif // MCRCLIENT_GLOBAL_H
