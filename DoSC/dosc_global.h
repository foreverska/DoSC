#ifndef DOSC_GLOBAL_H
#define DOSC_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(DOSC_LIBRARY)
#  define DOSCSHARED_EXPORT Q_DECL_EXPORT
#else
#  define DOSCSHARED_EXPORT Q_DECL_IMPORT
#endif

#endif // DOSC_GLOBAL_H
