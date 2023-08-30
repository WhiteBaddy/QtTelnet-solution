#ifndef QTTELNET_GLOBAL_H
#define QTTELNET_GLOBAL_H

#include <QtCore/qglobal.h>

#include <QDebug>

#if defined(QTTELNET_LIBRARY)
#  define QTTELNET_EXPORT Q_DECL_EXPORT
#else
#  define QTTELNET_EXPORT Q_DECL_IMPORT
#endif

#endif // QTTELNET_GLOBAL_H
