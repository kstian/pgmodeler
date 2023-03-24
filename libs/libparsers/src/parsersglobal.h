#ifndef PARSERS_GLOBAL_H
#define PARSERS_GLOBAL_H

#include <QtCore/QtGlobal>

#if defined(PARSERS_SYMBOLS)
	#define __libparsers Q_DECL_EXPORT
#else
	#define __libparsers Q_DECL_IMPORT
#endif

#endif
